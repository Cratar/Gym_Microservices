cmake_minimum_required(VERSION 3.10)
project(ViewAvailableRooms)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем путь к заголовкам ASIO
include_directories(${CMAKE_SOURCE_DIR}/asio/asio/include)

# Указываем путь к библиотеке Crow и redis-plus-plus
add_subdirectory(Crow)


# Поиск библиотеки PostgreSQL
find_package(PostgreSQL REQUIRED)

find_path(HIREDIS_HEADER hiredis)
target_include_directories(target PUBLIC ${HIREDIS_HEADER})

# Проверяем, что найдены необходимые библиотеки
if (NOT PostgreSQL_FOUND)
    message(FATAL_ERROR "PostgreSQL library not found.")
endif()

# Добавляем путь к заголовкам PostgreSQL
include_directories(${PostgreSQL_INCLUDE_DIRS})


# Указываем исходные файлы и библиотеки
add_executable(CheckGym ViewAvailableRooms.cpp)

# Линковка с необходимыми библиотеками: PostgreSQL, redis-plus-plus, Crow

find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(target PUBLIC ${REDIS_PLUS_PLUS_HEADER})


target_link_libraries(CheckGym PRIVATE ${PostgreSQL_LIBRARIES} Crow)
