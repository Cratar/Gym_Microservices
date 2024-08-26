
#include <iostream>
#include <string>
#include <libpq-fe.h>

#include "crow.h"

#define LOGIN_CONNECT "host=localhost port=5432 dbname=gyms user=postgres password=0000nN"

bool CheckGym() {



    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    std::string sql = "SELECT * FROM gyms";


    PGresult* res = PQexecParams(conn, sql.c_str(), 1, nullptr, nullptr, nullptr, nullptr, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        std::string storedPassword = PQgetvalue(res, 0, 0);
        PQclear(res);
        PQfinish(conn);

        return true;

    }
    else {
        PQclear(res);
        PQfinish(conn);
        return false;
    }

}

int main() {


    setlocale(LC_ALL, "Ru");

    crow::SimpleApp app;

    CROW_ROUTE(app, "/json")
        ([] {
        crow::json::wvalue Response({ {"Gyms", "Список залов"} });
        Response["message2"] = "Hello, World.. Again!";
        return Response;
            });
    app.port(8086).run();


}