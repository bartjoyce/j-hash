cmake_minimum_required(VERSION 3.14)
project(j-hash C)
set(CMAKE_CXX_STANDARD 14)

add_library(jhash STATIC
    src/jhash.h
    src/jhash.c
    src/jhash_encoding.c
    lib/sha256.h
    lib/sha256.c
    lib/base64.h
    lib/base64.c
)

add_library(jproof STATIC
    src/jhash.h
    src/jhash.c
    src/jhash_encoding.c
    src/jproof.h
    src/jproof_generate.c
    src/jproof_verify.c
    src/jproof_encoding.c
    lib/sha256.h
    lib/sha256.c
    lib/base64.h
    lib/base64.c
)

add_executable(jhash-test examples/jhash_test.c)
target_link_libraries(jhash-test jhash)

add_executable(jproof-test examples/jproof_test.c)
target_link_libraries(jproof-test jhash jproof)
