cmake_minimum_required(VERSION 3.5)
project (net)

set(CMAKE_CXX_STANDARD 20)

add_library(net_lib SHARED lib.cpp)
set_target_properties(net_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "net")
