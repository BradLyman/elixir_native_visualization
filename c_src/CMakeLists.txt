cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
include_directories("/usr/lib/erlang/usr/include")
add_library(NatVis SHARED ./first_nif.cpp)

