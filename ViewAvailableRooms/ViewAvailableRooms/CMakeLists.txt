cmake_minimum_required(VERSION 3.10)
project(ViewAvailableRooms)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Путь к заголовкам ASIO
include_directories(${CMAKE_SOURCE_DIR}/asio/asio/include)

# Путь к библиотекам Crow и redis-plus-plus
add_subdirectory(Crow)

# Поиск библиотеки PostgreSQL
find_package(PostgreSQL REQUIRED)

# Проверяем, что PostgreSQL найдена
if (NOT PostgreSQL_FOUND)
    message(FATAL_ERROR "PostgreSQL library not found.")
endif()

# Добавляем путь к заголовкам PostgreSQL
include_directories(${PostgreSQL_INCLUDE_DIRS})

# <------------ Установка пути к hiredis и redis-plus-plus --------------->
# Указываем путь к заголовкам и библиотекам Hiredis
include_directories(/usr/include/hiredis)
link_directories(/usr/lib/x86_64-linux-gnu)

# Указываем путь к заголовкам redis-plus-plus
include_directories(/app/redis++/src)
link_directories(/app/redis++/build)

# Добавляем исходный файл и библиотеки для линковки
add_executable(CheckGym ViewAvailableRooms.cpp)

# Линковка с PostgreSQL, Crow, Hiredis, redis-plus-plus
target_link_libraries(CheckGym PRIVATE ${PostgreSQL_LIBRARIES} /usr/lib/x86_64-linux-gnu/libhiredis.so /app/redis++/build/libredis++.so Crow)
