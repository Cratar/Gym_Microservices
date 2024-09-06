cmake_minimum_required(VERSION 3.10)
project(SportGym) 

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем путь к заголовкам ASIO
include_directories(${CMAKE_SOURCE_DIR}/asio/asio/include)

# Указываем путь к библиотеке Crow
add_subdirectory(Crow)
include_directories(${CMAKE_SOURCE_DIR}/Crow/include)  # если требуется путь к заголовкам

# Поиск библиотек PostgreSQL и OpenSSL
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)

# Добавляем путь к заголовкам PostgreSQL
include_directories(${PostgreSQL_INCLUDE_DIRS})

# Указываем исходные файлы и библиотеки
add_executable(SportGym
    AddingPages/AddingPages/AddingPages.cpp
    AuthorizationAdmin/AuthorizationAdmin/AuthorizationAdmin.cpp
    AuthorizationUser/AuthorizationUser/AuthorizationUser.cpp
    CreateDbTable/CreateDbTable/CreateDbTable.cpp
    EletingRecords/EletingRecords/EletingRecords.cpp
    RegistrationAdministrator/RegistrationAdministrator/RegistrationAdministrator.cpp
    RegistrationUser/RegistrationUser/RegistrationUser.cpp
    ViewAvailableRooms/ViewAvailableRooms/ViewAvailableRooms.cpp
)

# Линковка с необходимыми библиотеками
target_link_libraries(SportGym PRIVATE ${PostgreSQL_LIBRARIES} Crow OpenSSL::SSL)
