cmake_minimum_required(VERSION 3.23)
project(local_domain_chacker)

set(CMAKE_CXX_STANDARD 17)

add_executable(local_domain_chacker main.cpp)

find_package(ZLIB)
target_link_libraries(local_domain_chacker ZLIB::ZLIB)
