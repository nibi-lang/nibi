cmake_minimum_required(VERSION 3.5)
project (threads)

set(CMAKE_CXX_STANDARD 20)

find_package(libnibi REQUIRED)
include_directories(${LIBNIBI_INCLUDE_DIRS})

add_library(threads_lib SHARED lib.cpp)
target_link_libraries(threads_lib ${LIBNIBI_LIBRARIES})
set_target_properties(threads_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "threads")
