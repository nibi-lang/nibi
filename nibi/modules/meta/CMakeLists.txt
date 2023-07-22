cmake_minimum_required(VERSION 3.5)
project (meta)

set(CMAKE_CXX_STANDARD 20)

add_library(meta_lib SHARED lib.cpp)
set_target_properties(meta_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "meta")
