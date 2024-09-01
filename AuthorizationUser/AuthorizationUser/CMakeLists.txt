cmake_minimum_required(VERSION 3.10)
project(AuthorizationUser)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем путь к заголовкам ASIO
include_directories(${CMAKE_SOURCE_DIR}/asio/asio/include)

# Указываем путь к библиотеке Crow
add_subdirectory(Crow)

# Поиск библиотеки PostgreSQL и OpenSSL
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)

# Проверяем, что найдены необходимые библиотеки
if (NOT PostgreSQL_FOUND)
    message(FATAL_ERROR "PostgreSQL library not found.")
endif()

# Добавляем путь к заголовкам PostgreSQL
include_directories(${PostgreSQL_INCLUDE_DIRS})

# Указываем исходные файлы и библиотеки
add_executable(AuthorizationUser AuthorizationUser.cpp)

# Линковка с необходимыми библиотеками
target_link_libraries(AuthorizationUser PRIVATE ${PostgreSQL_LIBRARIES} Crow OpenSSL::SSL OpenSSL::Crypto)
