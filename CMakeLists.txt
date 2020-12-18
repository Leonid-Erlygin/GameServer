cmake_minimum_required(VERSION 3.13)
project(GameServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(GameServer main.cpp)


target_link_libraries(GameServer -lsfml-network)