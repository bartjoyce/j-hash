cmake_minimum_required(VERSION 3.14)
project(j-hash C)
set(CMAKE_CXX_STANDARD 14)

add_executable(jhash
    jhash_test.c
    jhash.c
    jhash.h
    sha256.h
    sha256.c
)
