cmake_minimum_required(VERSION 3.10)
project (math)

set(CMAKE_CXX_STANDARD 20)

add_library(math_lib SHARED lib.cpp)
set_target_properties(math_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "math")
