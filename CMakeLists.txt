cmake_minimum_required(VERSION 3.16)
project(nakacore CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Buscar dependencia OpenSSL para el hashing SHA-256
find_package(OpenSSL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

# Ejecutable principal del nodo nakacored
add_executable(nakacored
    src/main.cpp
    src/chain.cpp
)

# Vincular OpenSSL con el binario
target_link_libraries(nakacored PRIVATE OpenSSL::Crypto)
