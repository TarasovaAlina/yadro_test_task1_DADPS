cmake_minimum_required(VERSION 3.10)
project(AccelerometerSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Папка с заголовками
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Создаём исполняемые файлы
add_executable(server src/server.cpp)
add_executable(client_a src/client_a.cpp)
add_executable(client_b src/client_b.cpp)

# Указываем путь к заголовкам
target_include_directories(server PRIVATE ${INCLUDE_DIR})
target_include_directories(client_a PRIVATE ${INCLUDE_DIR})
target_include_directories(client_b PRIVATE ${INCLUDE_DIR})

# Подключаем pthread
target_link_libraries(server pthread)
target_link_libraries(client_a pthread)
target_link_libraries(client_b pthread)