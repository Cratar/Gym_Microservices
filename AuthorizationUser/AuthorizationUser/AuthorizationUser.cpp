﻿
#include <iostream>
#include <string>
#include <libpq-fe.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "crow.h"


#define LOGIN_CONNECT "host=localhost port=5432 dbname=reg_user user=postgres password=0000nN"


std::string hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();

    // Инициализация контекста с использованием SHA-256
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    // Обновление контекста данными пароля
    EVP_DigestUpdate(ctx, password.c_str(), password.length());
    // Финализация и получение хэша
    EVP_DigestFinal_ex(ctx, hash, NULL);

    // Освобождение памяти, выделенной для контекста
    EVP_MD_CTX_free(ctx);

    // Преобразование хэша в строку в шестнадцатеричном формате
    std::string hex_str;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", hash[i]);
        hex_str += buf;
    }
    return hex_str;
}

bool AuthorizationUser(const crow::json::rvalue& email, const crow::json::rvalue& password) {

    std::string emailS = email.s();
    std::string passwordS = password.s();

    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    std::string sql = "SELECT password FROM users WHERE email = $1";

    const char* paramValues[1] = { emailS.c_str() };

    PGresult* res = PQexecParams(conn, sql.c_str(), 1, nullptr, paramValues, nullptr, nullptr, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        std::string storedPassword = PQgetvalue(res, 0, 0);
        PQclear(res);
        PQfinish(conn);

        if (hashPassword(passwordS) == storedPassword) {
            std::cout << "Вы успешно авторизовались.\n";
            return true;
        }
        else {
            std::cout << "Неправильный email или пароль.\n";
            return false;
        }
    }
    else {
        PQclear(res);
        PQfinish(conn);
        std::cout << "Неправильный email или пароль.\n";
        return false;
    }

}



int main() {

    setlocale(LC_ALL, "Ru");

    crow::SimpleApp app;

    CROW_ROUTE(app, "/aut/User")
        .methods("GET"_method)
        ([](const crow::request& req) {

        crow::json::rvalue json_data = crow::json::load(req.body);

        if (!json_data)
            return crow::response(crow::status::BAD_REQUEST);

        crow::json::rvalue email = json_data["email"];
        crow::json::rvalue password = json_data["password"];

        bool flag = AuthorizationUser(email, password);

        if (flag) 
            return crow::response(200);
        
        return crow::response(crow::status::BAD_REQUEST);



            });
    app.port(8086).run();


}