/*
c++: 20
deps:
    - org.sw.demo.boost.asio
    - org.sw.demo.postgres.pq
*/

// see https://www.postgresql.org/docs/current/libpq-async.html

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <libpq-fe.h>

#define connection_string "user=postgres password=postgres"
#define connection_string_async "hostaddr=127.0.0.1 " connection_string
#define result "1"
#define query "SELECT " result ";"

boost::asio::awaitable<void> async(boost::asio::io_context &ctx) {
    using namespace boost::asio::ip;

    auto wait_for_socket = [&ctx](auto s, auto wait_type) -> boost::asio::awaitable<void> {
        // create socket with proper protocol v4/v6 (automate this)
        tcp::socket as(ctx, tcp::v4(), s);
        // wait for the condition in a coroutine
        co_await as.async_wait(wait_type, boost::asio::use_awaitable);
        // prevent socket to be closed by boost
        as.release();
    };

    // async resolve hostname goes here...
    // now we use returned ip addr
    auto conn = PQconnectStart(connection_string_async);
    if (conn) {
        //PQtrace(conn, stdout);
        if (auto status = PQstatus(conn); status == CONNECTION_STARTED) {
            auto s = PQsocket(conn);
            auto r = PGRES_POLLING_WRITING;
            while (r == PGRES_POLLING_WRITING || r == PGRES_POLLING_READING) {
                co_await wait_for_socket(s, r == PGRES_POLLING_WRITING ? tcp::socket::wait_write : tcp::socket::wait_read);
                r = PQconnectPoll(conn);
                s = PQsocket(conn); // socket might be changed after PQconnectPoll()
            }
            if (auto status = PQstatus(conn); status == CONNECTION_OK) {
                // query!
                // set non blocking query first
                if (!PQisnonblocking(conn) && PQsetnonblocking(conn, 1) == 0) {
                    // make huge query (huge number of simple quieries to test i/o with coroutines)
                    std::string q = query;
                    int n = 8192;
                    while (n--)
                        q += query;
                    if (PQsendQuery(conn, q.c_str())) {
                        // we can emit single rows to user
                        auto check_status = PGRES_TUPLES_OK;
                        if (PQsetSingleRowMode(conn)) {
                            check_status = PGRES_SINGLE_TUPLE;
                        } else {
                            std::cerr << "can't change to single row mode\n";
                        }
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
                                    auto res = PQgetResult(conn);
                                    if (res) {
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
                                    } else {
                                        break;
                                    }
                                }
                            }
                        } while (PQisBusy(conn));
                    } else {
                        std::cerr << PQerrorMessage(conn) << "\n";
                    }
                }
            }
        }
        //PQuntrace(conn);
        PQfinish(conn);
    }
    co_return;
}

int main() {
    boost::asio::io_context ctx;
    boost::asio::co_spawn(ctx, async(ctx), [](auto &&error) {
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
