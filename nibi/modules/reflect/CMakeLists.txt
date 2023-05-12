cmake_minimum_required(VERSION 3.5)
project (reflect)

set(CMAKE_CXX_STANDARD 20)

add_library(reflect_lib SHARED lib.cpp)
set_target_properties(reflect_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "reflect")