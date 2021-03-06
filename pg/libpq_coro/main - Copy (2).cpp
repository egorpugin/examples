/*
c++: 20
deps:
    - org.sw.demo.boost.asio
    - org.sw.demo.postgres.pq
*/

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <libpq-fe.h>

// see https://www.postgresql.org/docs/current/libpq-async.html

#define connection_string "user=postgres password=postgres"
#define connection_string_async "hostaddr=127.0.0.1 " connection_string
#define result "1"
#define QUERY "SELECT " result ";"

using namespace boost::asio::ip;
using namespace std::literals;

boost::asio::awaitable<void> wait_for_raw_socket(auto socket, auto wait_type) {
    // create socket with proper protocol v4/v6 (automate this)
    tcp::socket s(co_await boost::asio::this_coro::executor, tcp::v4(), socket);
    // wait for the condition in a coroutine
    co_await s.async_wait(wait_type, boost::asio::use_awaitable);
    // prevent socket to be closed by boost
    s.release();
};

struct connection {
    using coro_frame_t = boost::asio::detail::awaitable_frame_base<boost::asio::any_io_executor>;

    PGconn *conn;

    connection(auto &&conn_string) {
        conn = PQconnectStart(conn_string);
        if (!conn)
            throw std::runtime_error("cannot create pg connection to "s + conn_string);
        if (auto status = PQstatus(conn); status != CONNECTION_STARTED)
            throw std::runtime_error("cannot create pg connection to "s + conn_string + ". error = " + PQerrorMessage(conn));
    }
    ~connection() {
        PQfinish(conn);
    }

    boost::asio::awaitable<void> connect() const {
        if (auto status = PQstatus(conn); status != CONNECTION_STARTED)
            throw std::runtime_error("wrong connection status"s);
        auto s = PQsocket(conn);
        auto r = PGRES_POLLING_WRITING;
        while (r == PGRES_POLLING_WRITING || r == PGRES_POLLING_READING) {
            co_await wait_for_raw_socket(s, r == PGRES_POLLING_WRITING ? tcp::socket::wait_write : tcp::socket::wait_read);
            r = PQconnectPoll(conn);
            s = PQsocket(conn); // socket might be changed after PQconnectPoll()
        }
    }
    boost::asio::awaitable<void> query(const std::string &q, bool single_row_mode = false) const {
        auto check_status = single_row_mode ? PGRES_SINGLE_TUPLE : PGRES_TUPLES_OK;
        PGresult *res = nullptr;
        coro_frame_t *child_frame = nullptr;
        coro_frame_t *this_frame = nullptr;
        auto executor = co_await boost::asio::this_coro::executor;
        co_await [&this_frame, &executor](auto frame) {
            this_frame = frame;
            return &executor;
        };
        // start io loop with server
        co_await send_query(&res, &child_frame, q, single_row_mode);
        while (1) {
            if (!res)
                break;
            if (auto status = PQresultStatus(res); status == check_status || status == PGRES_TUPLES_OK) {
                if (check_status == PGRES_TUPLES_OK || status == check_status) {
                    auto val = PQgetvalue(res, 0, 0);
                    if (strcmp(val, result) == 0) {
                        std::cout << "ok\n";
                    } else {
                        std::cerr << "error\n";
                    }
                }
            } else {
                std::cerr << PQresStatus(status) << "\n";
            }
            PQclear(res);

            // return to the child frame
            child_frame->resume();
            /*co_await[child_frame](auto frame) {
                frame->push_frame(child_frame);
                return frame->detach_thread();
            };*/
        }
    }

private:
    boost::asio::awaitable<void> send_query(PGresult **res, coro_frame_t **this_frame, const std::string &q, bool single_row_mode = false) const {
        if (auto status = PQstatus(conn); status != CONNECTION_OK)
            throw std::runtime_error("wrong connection status"s);
        if (!PQisnonblocking(conn) && PQsetnonblocking(conn, 1) == -1)
            throw std::runtime_error("can't set nonblocking mode"s);

        auto s = PQsocket(conn);
        if (PQsendQuery(conn, q.c_str())) {
            if (single_row_mode && !PQsetSingleRowMode(conn))
                throw std::runtime_error("can't change to single row mode"s);
            //
            while (auto r = PQflush(conn)) {
                if (r == -1)
                    throw std::runtime_error("PQflush error: "s + PQerrorMessage(conn));
                co_await wait_for_raw_socket(s, tcp::socket::wait_write);
            }
            // read response
            do {
                co_await wait_for_raw_socket(s, tcp::socket::wait_read);
                if (PQconsumeInput(conn)) {
                    while (!PQisBusy(conn)) {
                        *res = PQgetResult(conn);
                        if (!*res)
                            break;
                        // switch to parent
                        co_await [&this_frame](auto frame) {
                            *this_frame = frame;
                            frame->pop_frame();
                            return frame->detach_thread();
                        };
                    }
                }
            } while (PQisBusy(conn));
        } else {
            throw std::runtime_error("PQsendQuery error: "s + PQerrorMessage(conn));
        }
    }
};

boost::asio::awaitable<void> async2() {
    // async resolve hostname goes here...
    // now we use returned ip addr
    connection c{connection_string_async};
    co_await c.connect();

    // make huge query (huge number of simple quieries to test i/o with coroutines)
    std::string q = QUERY;
    int n = 81920;
    while (n--)
        q += QUERY;
    co_await c.query("SELECT 1 UNION SELECT 2"s, true);
}

int main() {
    boost::asio::io_context ctx;
    boost::asio::co_spawn(ctx, async2(), [](auto &&error) {
        if (!error)
            return;
        try {
            std::rethrow_exception(error);
        } catch (std::exception &e) {
            std::cerr << e.what() << "\n";
        }
    });
    ctx.run();
}
