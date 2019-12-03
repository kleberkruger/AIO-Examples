cmake_minimum_required(VERSION 3.15)
project(AIO_Examples C)

set(CMAKE_C_STANDARD 11)

add_executable(AIO_Examples main.c kqueue.c select.c)