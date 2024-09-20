#include <iostream>
#include <string>
#include <libpq-fe.h>
#include "crow.h"
#include <sw/redis++/redis++.h>

using namespace sw::redis;

// Данные для подключения к PostgreSQL

#define LOGIN_CONNECT "host=host.docker.internal port=5432 dbname=sport_gyms user=postgres password=0000nN"

// Функция для получения данных из базы данных и преобразования их в JSON
crow::json::wvalue GetGymsJson(Redis& redis) {
    // Ключ для Redis
    std::string redis_key = "gyms_data";

    try {
        // Проверяем кэш Redis
        auto cached_data = redis.get(redis_key);
        if (cached_data) {
            std::cout << "Данные взяты из Redis кэша." << std::endl;
            return crow::json::load(*cached_data); // Возвращаем закэшированные данные
        }
    }
    catch (const Error& error) {
        std::cerr << "Ошибка Redis: " << error.what() << std::endl;

        PGconn* conn = PQconnectdb(LOGIN_CONNECT);
        const char* sql = "SELECT gym_name, address, reservation_date, reservation_time FROM gyms";
        PGresult* res = PQexec(conn, sql);
        std::cerr << "Ошибка Redis: Данные взяты из БД " << error.what() << std::endl;

    }

    // Устанавливаем соединение с базой данных
    PGconn* conn = PQconnectdb(LOGIN_CONNECT);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка подключения к базе данных: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return crow::json::wvalue{ {"error", "Ошибка подключения к базе данных"} };
    }

    // SQL-запрос
    const char* sql = "SELECT gym_name, address, reservation_date, reservation_time FROM gyms";
    PGresult* res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка SQL-запроса: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return crow::json::wvalue{ {"error", "Ошибка SQL-запроса"} };
    }

    // Формируем JSON-ответ
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    crow::json::wvalue jsonResponse;
    crow::json::wvalue& gymsArray = jsonResponse["gyms"] = crow::json::wvalue::list();

    for (int i = 0; i < rows; i++) {
        crow::json::wvalue gym;
        for (int j = 0; j < cols; j++) {
            std::string columnName = PQfname(res, j);
            std::string value = PQgetvalue(res, i, j);
            gym[columnName] = value;
        }
        gymsArray[i] = std::move(gym); // Добавляем элемент в JSON массив
    }

    // Освобождаем ресурсы
    PQclear(res);
    PQfinish(conn);

    // Кэшируем результат в Redis
    try {
        redis.set(redis_key, jsonResponse.dump());
        redis.expire(redis_key, std::chrono::seconds(60));  // Устанавливаем время жизни кэша
    }
    catch (const Error& error) {
        std::cerr << "Ошибка кэширования данных в Redis: " << error.what() << std::endl;
    }

    return jsonResponse;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    crow::SimpleApp app;



    // Подключение к Redis
    Redis redis("tcp://redis_container:6379");

    // Маршрут для получения данных о залах
    CROW_ROUTE(app, "/check/gym")
        ([&redis]() {

        crow::json::wvalue result;

        std::thread th([&redis , &result]() 
            {
                result = GetGymsJson(redis);
            });
        th.join();
        return result;

            });

    // Запуск сервера на порту 80
    app.port(80).multithreaded().run();

}
