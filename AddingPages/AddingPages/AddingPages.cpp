
#include <iostream>
#include <string>
#include <libpq-fe.h>


#include "crow.h"

#define LOGIN_CONNECT "host=localhost port=5432 dbname=Gym user=postgres password=0000nN"


bool AddingPages(const std::string& gym_name, const std::string& address, const std::string& reservation_date, const std::string& reservation_time, const std::string& hall_booked) {

    // Устанавливаем соединение
    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    // Проверяем состояние соединения
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    // Параметризованный SQL-запрос
    const char* sql = "INSERT INTO gyms (gym_name, address, reservation_date, reservation_time, hall_booked) VALUES ($1, $2, $3, $4, $5);";

    // Преобразуем логическое значение в строку
    const char* paramValues[5] = { gym_name.c_str(), address.c_str(), reservation_date.c_str(), reservation_time.c_str(), hall_booked.c_str() };

    // Выполняем SQL-запрос
    PGresult* res = PQexecParams(conn, sql, 5, nullptr, paramValues, nullptr, nullptr, 0);

    // Проверяем успешность выполнения запроса
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Data insertion failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return false;
    }

    // Очищаем результат и закрываем соединение
    PQclear(res);
    PQfinish(conn);

    std::cout << "Добавление зала прошло успешно!" << std::endl;

    return true;
}


int main() {


    setlocale(LC_ALL, "Ru");

    crow::SimpleApp app;

    CROW_ROUTE(app, "/adding/pages")
        .methods("POST"_method)
        ([](const crow::request& req) {
        crow::json::rvalue json_data = crow::json::load(req.body);
        if (!json_data)
            return crow::response(crow::status::BAD_REQUEST);

        std::string gym_name = json_data["gym_name"].s();
        std::string address = json_data["address"].s();
        std::string reservation_date = json_data["reservation_date"].s();
        std::string reservation_time = json_data["reservation_time"].s();
        std::string hall_booked = json_data["hall_booked"].s();


        bool flag = AddingPages(gym_name, address, reservation_date, reservation_time, hall_booked);

        if (flag)
            return crow::response(200);

        return crow::response(crow::status::BAD_REQUEST);


            });
    app.port(8086).run();


}