
#include <iostream>
#include <string>
#include <libpq-fe.h>

#include <openssl/evp.h>
#include <openssl/sha.h>

#include "crow.h"

#define LOGIN_CONNECT "host=host.docker.internal port=5432 dbname=reg_user user=postgres password=0000nN"

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

bool RegistrationUser(const std::string& name, const std::string& surname, const std::string& email, const std::string& age, const std::string& password) {


    // Хэшируем пароль и сохраняем результат
    std::string hashedPassword = hashPassword(password);

    // Устанавливаем соединение
    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    // Проверяем состояние соединения
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }

    // Параметризованный SQL-запрос
    const char* sql = "INSERT INTO users (name, surname, email, age, password) VALUES ($1, $2, $3, $4, $5) ;";

    // Параметры для SQL-запроса
    const char* paramValues[5] = { name.c_str(), surname.c_str(), email.c_str(), age.c_str() , hashedPassword.c_str() };

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

    std::cout << "Регистрация прошла успешно!" << std::endl;

    return true;
}


int main() {


    setlocale(LC_ALL, "ru_RU.UTF-8");

    crow::SimpleApp app;

    CROW_ROUTE(app, "/reg/User")
        .methods("POST"_method)
        ([](const crow::request& req) {
        crow::json::rvalue json_data = crow::json::load(req.body);
        if (!json_data)
            return crow::response(crow::status::BAD_REQUEST);

        std::string name = json_data["name"].s();
        std::string surname = json_data["surname"].s();
        std::string email = json_data["email"].s();
        std::string age = json_data["age"].s();
        std::string password = json_data["password"].s();


        bool flag = RegistrationUser(name, surname, email, age, password);

        if (flag) 
            return crow::response(200);

        return crow::response(crow::status::BAD_REQUEST);


            });
    app.port(8082).run();


}