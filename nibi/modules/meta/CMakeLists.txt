cmake_minimum_required(VERSION 3.10)
project (meta)

set(CMAKE_CXX_STANDARD 20)

find_package(libnibi REQUIRED)
include_directories(${LIBNIBI_INCLUDE_DIRS})

add_library(meta_lib SHARED lib.cpp)
target_link_libraries(meta_lib ${LIBNIBI_LIBRARIES})
set_target_properties(meta_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "meta")
