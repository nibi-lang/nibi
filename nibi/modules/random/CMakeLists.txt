cmake_minimum_required(VERSION 3.5)
project (random)

set(CMAKE_CXX_STANDARD 20)

add_library(random_lib SHARED lib.cpp)
set_target_properties(random_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "random")