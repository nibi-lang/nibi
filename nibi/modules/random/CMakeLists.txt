cmake_minimum_required(VERSION 3.10)
project (random)

set(CMAKE_CXX_STANDARD 20)

find_package(libnibi REQUIRED)
include_directories(${LIBNIBI_INCLUDE_DIRS})

add_library(random_lib SHARED lib.cpp)
target_link_libraries(random_lib ${LIBNIBI_LIBRARIES})
set_target_properties(random_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "random")
