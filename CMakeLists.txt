cmake_minimum_required(VERSION 3.14)
project(mongoose_server C)

set(CMAKE_C_STANDARD 11)

add_executable(mongoose_server WebServer.c mongoose.c mongoose.h)