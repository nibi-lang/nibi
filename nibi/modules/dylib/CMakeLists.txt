cmake_minimum_required(VERSION 3.5)

project (dylib)

set(CMAKE_CXX_STANDARD 20)

add_library(dylib_lib 
  SHARED 
  lib.cpp 
  RLL/rll_wrapper.cpp)

set_target_properties(dylib_lib 
  PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "dylib")
