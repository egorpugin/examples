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

template <typename ReturnType, typename Executor>
struct yield_awaitable_frame;

template <typename ReturnType, typename Executor = boost::asio::any_io_executor>
struct yield_awaitable {
    typedef yield_awaitable_frame<ReturnType, Executor> promise_type;
    typedef Executor executor_type;

    constexpr yield_awaitable() noexcept : frame_(nullptr) {
    }
    yield_awaitable(yield_awaitable &&other) noexcept : frame_(std::exchange(other.frame_, nullptr)) {
    }
    ~yield_awaitable() {
        if (frame_)
            frame_->destroy();
    }

    bool valid() const noexcept {
        return !!frame_;
    }

    bool await_ready() const noexcept {
        return false;
    }
    template <typename U>
    void await_suspend(std::coroutine_handle<boost::asio::detail::awaitable_frame<U, Executor>> h) {
        frame_->push_frame(&h.promise());
    }
    void await_suspend(std::coroutine_handle<promise_type> h) {
        if (yield) {
            //h.promise().resume_caller();
            //frame_->push_frame(&h.promise());
            frame_->pop_frame();
        } else {
            frame_->push_frame(&h.promise());
        }
    }
    ReturnType await_resume() {
        return yield_awaitable(static_cast<yield_awaitable &&>(*this)).frame_->get();
    }

    operator std::coroutine_handle<boost::asio::detail::awaitable_frame<ReturnType, Executor>>() const {
        return frame_;
    }

private:
    template <typename> friend class boost::asio::detail::awaitable_thread;
    template <typename, typename> friend struct yield_awaitable_frame;

    // Not copy constructible or copy assignable.
    yield_awaitable(const yield_awaitable&) = delete;
    yield_awaitable& operator=(const yield_awaitable&) = delete;

    // Construct the awaitable from a coroutine's frame object.
    explicit yield_awaitable(yield_awaitable_frame<ReturnType, Executor> *a, bool yield) : frame_(a), yield(yield) {
    }

    yield_awaitable_frame<ReturnType, Executor> *frame_;
    bool yield = false;
};

template <typename ReturnType, typename Executor>
struct yield_awaitable_frame : boost::asio::detail::awaitable_frame_base<Executor> {
    yield_awaitable_frame() noexcept {
    }
    yield_awaitable_frame(yield_awaitable_frame &&other) noexcept : awaitable_frame_base<Executor>(std::move(other)) {
    }
    ~yield_awaitable_frame() {
        if (has_result_)
            static_cast<ReturnType *>(static_cast<void *>(result_))->~ReturnType();
    }

    auto get_return_object() noexcept {
        this->coro_ = std::coroutine_handle<yield_awaitable_frame>::from_promise(*this);
        return yield_awaitable<ReturnType, Executor>(this, false);
    }

    template <typename U>
    void return_value(U &&u) {
        new (&result_) ReturnType(std::forward<U>(u));
        has_result_ = true;
        ended_ = true;
    }
    template <typename... Us>
    void return_values(Us &&...us) {
        this->return_value(std::forward_as_tuple(std::forward<Us>(us)...));
    }

    ReturnType get() {
        this->caller_ = nullptr;
        this->rethrow_exception();
        return std::move(*static_cast<ReturnType *>(static_cast<void *>(result_)));
    }

    template <typename U>
    auto yield_value(U &&u) {
        if (has_result_)
            static_cast<ReturnType *>(static_cast<void *>(result_))->~ReturnType();
        new (&result_) ReturnType(std::forward<U>(u));
        has_result_ = true;
        return yield_awaitable<ReturnType, Executor>(this, true);
    }

    void resume_caller() {
        this->caller_->resume();
    }

private:
    alignas(ReturnType) unsigned char result_[sizeof(ReturnType)];
    bool has_result_ = false;
    bool ended_ = false;
};

template <typename Executor>
struct yield_awaitable_frame<void, Executor> : boost::asio::detail::awaitable_frame_base<Executor> {
    auto get_return_object() {
        this->coro_ = std::coroutine_handle<yield_awaitable_frame>::from_promise(*this);
        return yield_awaitable<void, Executor>(this, false);
    };

    void return_void() {
    }

    void get() {
        this->caller_ = nullptr;
        this->rethrow_exception();
    }
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
        // start io loop with server
        auto query_coro = send_query(q, single_row_mode);
        /*while (auto res = co_await query_coro) {
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
        }*/
    }

private:
    yield_awaitable<PGresult *> send_query(const std::string &q, bool single_row_mode = false) const {
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
                        auto res = PQgetResult(conn);
                        if (!res)
                            break;
                        co_yield res;
                    }
                }
            } while (PQisBusy(conn));
        } else {
            throw std::runtime_error("PQsendQuery error: "s + PQerrorMessage(conn));
        }
        co_return nullptr;
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
