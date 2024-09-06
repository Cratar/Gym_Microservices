#include <iostream>
#include <string>
#include <libpq-fe.h>
#include "crow.h"

// host.docker.internal   localhost
#define LOGIN_CONNECT_CREATE_DB "host=host.docker.internal  port=5432 dbname=postgres user=postgres password=0000nN"
#define LOGIN_CONNECT "host=host.docker.internal  port=5432 dbname=sport_gyms user=postgres password=0000nN"


// Функция для создания базы данных Gym
bool CreateDatabase() {
    PGconn* conn = PQconnectdb(LOGIN_CONNECT_CREATE_DB);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    const char* sql = R"(
        CREATE DATABASE sport_gyms
    )";

    PGresult* res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Database creation failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return false;
    }

    PQclear(res);
    PQfinish(conn);

    std::cout << "Database 'sport_gyms' created successfully!" << std::endl;
    return true;
}

// Функция для создания таблицы gyms в базе данных Gym
bool CreateTable() {
    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS gyms (
            gym_id SERIAL PRIMARY KEY,
            gym_name varchar(50) NOT NULL,
            address varchar(100) NOT NULL,
            reservation_date date NULL,
            reservation_time time NULL

        );

       CREATE TABLE IF NOT EXISTS admin (
          id_admin SERIAL PRIMARY KEY,
          name varchar(30) NOT NULL,
          surname varchar(30) NULL,
          email varchar(80) NOT NULL,
          post varchar(30) NOT NULL,
          age int4 NULL,
          password varchar(100) NOT NULL

        );


        CREATE TABLE IF NOT EXISTS public.users (
          id_user SERIAL PRIMARY KEY,
          name varchar(100) NOT NULL,
          surname varchar(100) NULL,
          email varchar(100) NOT NULL,
          age int4 NULL,
          password varchar(100) NOT NULL

        );

    )";

    PGresult* res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Table creation failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return false;
    }

    PQclear(res);
    PQfinish(conn);

    std::cout << "Table 'sport_gyms' created successfully!" << std::endl;
    return true;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    crow::SimpleApp app;

    CROW_ROUTE(app, "/create_db")
        ([] {
        if (CreateDatabase())
            return crow::response(200);
        else
            return crow::response(crow::status::BAD_REQUEST);
            });

    CROW_ROUTE(app, "/create_table")
        ([] {
        if (CreateTable())
            return crow::response(200);
        else
            return crow::response(crow::status::BAD_REQUEST);
            });

    app.port(80).multithreaded().run();
}
