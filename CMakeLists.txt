cmake_minimum_required(VERSION 3.25)
project(asio_sandbox)

set(CMAKE_CXX_STANDARD 20)

find_package(asio CONFIG REQUIRED)

add_executable(asio_sandbox main.cpp)
target_link_libraries(asio_sandbox asio::asio)
