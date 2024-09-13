#include <iostream>
#include <string>
#include <libpq-fe.h>
#include "crow.h"
// Test
//host.docker.internal
#define LOGIN_CONNECT "host=host.docker.internal port=5432 dbname=sport_gyms user=postgres password=0000nN"

bool DeletingRecords(const std::string& gym_name, const std::string& reservation_date) {
    // Устанавливаем соединение
    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    // Проверяем состояние соединения
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    // Параметризованный SQL-запрос
    const char* sql = "DELETE FROM gyms WHERE gym_name = $1 AND reservation_date = $2;";

    const char* paramValues[2] = { gym_name.c_str(), reservation_date.c_str() };

    // Выполняем SQL-запрос
    PGresult* res = PQexecParams(conn, sql, 2, nullptr, paramValues, nullptr, nullptr, 0);

    // Проверяем успешность выполнения запроса
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Data deletion failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return false;
    }

    // Очищаем результат и закрываем соединение
    PQclear(res);
    PQfinish(conn);

    std::cout << "Удаление зала прошло успешно!" << std::endl;
    return true;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    crow::SimpleApp app;

    CROW_ROUTE(app, "/deleting/records")
        .methods("POST"_method)
        ([](const crow::request& req) {
            auto json_data = crow::json::load(req.body);
            if (!json_data) {
                return crow::response(crow::status::BAD_REQUEST);
            }

            if (!json_data.has("gym_name") || !json_data.has("reservation_date")) {
                return crow::response(crow::status::BAD_REQUEST);
            }

            std::string gym_name = json_data["gym_name"].s();
            std::string reservation_date = json_data["reservation_date"].s();

            bool flag = DeletingRecords(gym_name, reservation_date);

            if (flag) {
                return crow::response(200);
            }

            return crow::response(crow::status::BAD_REQUEST);
        });

    app.port(80).multithreaded().run();

}
