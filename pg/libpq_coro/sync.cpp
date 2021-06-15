/*
c++: 20
deps:
    - org.sw.demo.postgres.pq
*/

#include <iostream>

#include <libpq-fe.h>

#define connection_string "user=postgres password=postgres"
#define connection_string_async "hostaddr=127.0.0.1 " connection_string
#define result "1"
#define query "SELECT " result ";"

int main() {
    auto conn = PQconnectdb(connection_string);
    auto status = PQstatus(conn);
    if (status == CONNECTION_OK) {
        auto res = PQexec(conn, query);
        if (res) {
            if (auto status = PQresultStatus(res); status == PGRES_TUPLES_OK) {
                auto val = PQgetvalue(res, 0, 0);
                if (strcmp(val, result) == 0) {
                    std::cout << "ok" << std::endl;
                } else {
                    std::cerr << "error\n";
                }
            } else {
                std::cerr << PQresStatus(status) << "\n";
            }
            PQclear(res);
        }
    }
    PQfinish(conn);
}
