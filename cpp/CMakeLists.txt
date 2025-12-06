cmake_minimum_required(VERSION 3.10.0)
project(proj VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenSSL REQUIRED)

add_executable(proj src/main.cpp)

target_compile_options(proj PRIVATE -Wall -Wextra)
target_link_libraries(proj PRIVATE OpenSSL::SSL OpenSSL::Crypto)
