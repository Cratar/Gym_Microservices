
#include <iostream>
#include <string>
#include <libpq-fe.h>


#include "crow.h"

#define LOGIN_CONNECT "host=localhost port=5432 dbname=Gym user=postgres password=0000nN"


bool AddingPages(const crow::json::rvalue& gym_name, const crow::json::rvalue& address, const crow::json::rvalue& reservation_date, const crow::json::rvalue& reservation_time, const crow::json::rvalue& hall_booked) {

    std::string gym_nameS = gym_name.s();
    std::string addressS = address.s();
    std::string reservation_dateS = reservation_date.s();
    std::string reservation_timeS = reservation_time.s();
    std::string hall_bookedS = hall_booked.s();

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
    //std::string hall_booked_str = hall_bookedB ? "true" : "false";

    const char* paramValues[5] = { gym_nameS.c_str(), addressS.c_str(), reservation_dateS.c_str(), reservation_timeS.c_str(), hall_bookedS.c_str() };

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

        crow::json::rvalue gym_name = json_data["gym_name"];
        crow::json::rvalue address = json_data["address"];
        crow::json::rvalue reservation_date = json_data["reservation_date"];
        crow::json::rvalue reservation_time = json_data["reservation_time"];
        crow::json::rvalue hall_booked = json_data["hall_booked"];


        bool flag = AddingPages(gym_name, address, reservation_date, reservation_time, hall_booked);

        if (flag)
            return crow::response(200);

        return crow::response(crow::status::BAD_REQUEST);


            });
    app.port(8086).run();


}