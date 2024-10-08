﻿#include <iostream>
#include <string>
#include <libpq-fe.h>
#include "crow.h"
// host.docker.internal   localhost
#define LOGIN_CONNECT "host=host.docker.internal port=5432 dbname=sport_gyms user=postgres password=0000nN"


// Функция для добавления записей в таблицу gyms
bool AddingPages(const std::string& gym_name, const std::string& address, const std::string& reservation_date, const std::string& reservation_time) {
    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    const char* sql = "INSERT INTO gyms (gym_name, address, reservation_date, reservation_time) VALUES ($1, $2, $3, $4);";
    const char* paramValues[4] = { gym_name.c_str(), address.c_str(), reservation_date.c_str(), reservation_time.c_str() };

    PGresult* res = PQexecParams(conn, sql, 4, nullptr, paramValues, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Data insertion failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return false;
    }

    PQclear(res);
    PQfinish(conn);

    std::cout << "Добавление зала прошло успешно!" << std::endl;
    return true;
}


int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
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

        bool success = AddingPages(gym_name, address, reservation_date, reservation_time);

        if (success)
            return crow::response(200);
        else
            return crow::response(crow::status::BAD_REQUEST);
            });


    app.port(80).multithreaded().run();
}
