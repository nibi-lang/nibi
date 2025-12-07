cmake_minimum_required(VERSION 3.10)
project (module)

set(CMAKE_CXX_STANDARD 20)

add_library(module_lib SHARED lib.cpp)
set_target_properties(module_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "module")