cmake_minimum_required(VERSION 3.25)
project(asio_sandbox)

set(CMAKE_CXX_STANDARD 20)

option(ASIO_IO_URING "Use io_uring" OFF)

find_package(asio CONFIG REQUIRED)

if (ASIO_IO_URING)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(uring REQUIRED IMPORTED_TARGET liburing)
endif ()

add_executable(asio_sandbox main.cpp)
target_link_libraries(asio_sandbox asio::asio)
if (ASIO_IO_URING)
    target_link_libraries(asio_sandbox PkgConfig::uring)
    target_compile_definitions(asio_sandbox PRIVATE ASIO_HAS_IO_URING)
endif ()
