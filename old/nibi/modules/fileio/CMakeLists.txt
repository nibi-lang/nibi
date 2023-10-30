cmake_minimum_required(VERSION 3.5)
project (fileio)

set(CMAKE_CXX_STANDARD 20)

find_package(libnibi REQUIRED)
include_directories(${LIBNIBI_INCLUDE_DIRS})

add_library(fileio_lib SHARED lib.cpp)
target_link_libraries(fileio_lib ${LIBNIBI_LIBRARIES})
set_target_properties(fileio_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "fileio")
