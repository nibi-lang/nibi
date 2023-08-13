cmake_minimum_required(VERSION 3.5)

project (cpp_module)

set(CMAKE_CXX_STANDARD 20)

# Create a library with the module code
add_library(cpp_module_lib SHARED lib.cpp)

# Set the name of the output file to match the name of the module with the ".lib" extension
# output name must match the name of the module
set_target_properties(cpp_module_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "cpp_module")