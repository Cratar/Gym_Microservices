
#include <iostream>
#include <string>
#include <libpq-fe.h>

#include "crow.h"

#define LOGIN_CONNECT "host=host.docker.internal port=5432 dbname=Gym user=postgres password=0000nN"

crow::json::wvalue GetGymsJson() {
    // Устанавливаем соединение с базой данных
    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return crow::json::wvalue{ {"error", "Database connection failed"} };
    }

    // Выполняем SQL-запрос
    const char* sql = "SELECT gym_name, address, reservation_date , reservation_time , hall_booked FROM gyms";
    PGresult* res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SELECT failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return crow::json::wvalue{ {"error", "SQL query failed"} };
    }

    // Получаем количество строк и столбцов в результате запроса
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    // Создаем JSON-ответ
    crow::json::wvalue jsonResponse;
    crow::json::wvalue& gymsArray = jsonResponse["gyms"] = crow::json::wvalue::list();

    // Заполняем JSON-ответ данными из результата запроса
    for (int i = 0; i < rows; i++) {
        crow::json::wvalue gym;
        for (int j = 0; j < cols; j++) {
            std::string columnName = PQfname(res, j);
            std::string value = PQgetvalue(res, i, j);
            gym[columnName] = value;
        }

       
        gymsArray[i] = std::move(gym);  // Используем индексирование массива
    }

    // Освобождаем ресурсы
    PQclear(res);
    PQfinish(conn);

    return jsonResponse;
}

int main() {


    setlocale(LC_ALL, "ru_RU.UTF-8");


    crow::SimpleApp app;

    CROW_ROUTE(app, "/check/gym")
        ([] {
        return GetGymsJson();
            });

           
    app.port(8081).run();


}