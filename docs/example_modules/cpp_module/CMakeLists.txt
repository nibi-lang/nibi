cmake_minimum_required(VERSION 3.5)

project (cpp_module)

set(CMAKE_CXX_STANDARD 20)

find_package(libnibi REQUIRED)
include_directories(${LIBNIBI_INCLUDE_DIRS})

add_library(cpp_module_lib SHARED lib.cpp)
target_link_libraries(cpp_module_lib ${LIBNIBI_LIBRARIES})
set_target_properties(cpp_module_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "cpp_module")