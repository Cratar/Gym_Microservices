
#include <iostream>
#include <string>
#include <libpq-fe.h>

#include <openssl/evp.h>
#include <openssl/sha.h>

#include "crow.h"

#define LOGIN_CONNECT "host=localhost port=5432 dbname=reg_admin user=postgres password=0000nN"

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

bool RegistrationAdministrator(const crow::json::rvalue& name, const crow::json::rvalue& surname, const crow::json::rvalue& email, const crow::json::rvalue& post,const crow::json::rvalue& age, const crow::json::rvalue& password) {

    std::string nameS = name.s();
    std::string surnameS = surname.s();
    std::string emailS = email.s();
    std::string postS = post.s();
    std::string ageS = age.s();
    std::string passwordS = password.s();

    // Хэшируем пароль и сохраняем результат
    std::string hashedPassword = hashPassword(passwordS);

    // Устанавливаем соединение
    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    // Проверяем состояние соединения
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    // Параметризованный SQL-запрос
    const char* sql = "INSERT INTO admin (name, surname, email, post, age, password) VALUES ($1, $2, $3, $4, $5, $6);";

    // Параметры для SQL-запроса
    const char* paramValues[6] = { nameS.c_str(), surnameS.c_str(), emailS.c_str(), postS.c_str() ,ageS.c_str() , hashedPassword.c_str() };

    // Выполняем SQL-запрос
    PGresult* res = PQexecParams(conn, sql, 6, nullptr, paramValues, nullptr, nullptr, 0);

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

    std::cout << "Регистрация прошла успешно!" << std::endl;

    return true;
}


int main() {


    setlocale(LC_ALL, "Ru");

    crow::SimpleApp app;

    CROW_ROUTE(app, "/reg/Admin")
        .methods("POST"_method)
        ([](const crow::request& req) {
        crow::json::rvalue json_data = crow::json::load(req.body);
        if (!json_data)
            return crow::response(crow::status::BAD_REQUEST);

        crow::json::rvalue name = json_data["name"];
        crow::json::rvalue surname = json_data["surname"];
        crow::json::rvalue email = json_data["email"];
        crow::json::rvalue post = json_data["post"];
        crow::json::rvalue age = json_data["age"];
        crow::json::rvalue password = json_data["password"];


        bool flag = RegistrationAdministrator(name, surname, email, post, age, password);

        if (flag) 
            return crow::response(200);

        return crow::response(crow::status::BAD_REQUEST);



            });
    app.port(8086).run();


}