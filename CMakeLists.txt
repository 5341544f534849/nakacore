cmake_minimum_required(VERSION 3.10)
project(nakacored)

set(CMAKE_CXX_STANDARD 17)

add_executable(nakacored main.cpp blockchain.cpp wallet.cpp utxo.cpp mempool.cpp merkle.cpp)
