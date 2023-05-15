cmake_minimum_required(VERSION 3.5)
project (sys)

set(CMAKE_CXX_STANDARD 20)

add_library(sys_lib SHARED lib.cpp)
set_target_properties(sys_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "sys")