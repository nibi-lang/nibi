cmake_minimum_required(VERSION 3.5)

project (libnibi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

option(COMPILE_TESTS   "Execute unit tests" ON)
option(WITH_ASAN       "Compile with ASAN" OFF)

#
# Setup build type 'Release vs Debug'
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

#
# Setup ASAN
#
if(WITH_ASAN)
  set(CMAKE_CX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

include(${PROJECT_SOURCE_DIR}/cmake/SetEnv.cmake)

include_directories("libnibi")

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DNDEBUG)
endif()

set(NIBI_SOURCES
  ${PROJECT_SOURCE_DIR}/libnibi/cell.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/environment.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/source.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/modules.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/RLL/rll_wrapper.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/builtins.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/lambdas.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/arithmetic.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/environment_modifiers.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/asserts.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/list_commands.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/bitwise.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/comparison.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/common.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/env_cell.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/builtins/excepts.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter/interpreter.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/front/intake.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/front/token.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/platform.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/error.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/interpreter_factory.cpp
  ${PROJECT_SOURCE_DIR}/libnibi/module_factory.cpp
)

set(SOURCES
    ${NIBI_SOURCES}
)

#
# Configure Library
#
include(${PROJECT_SOURCE_DIR}/cmake/LibraryConfig.cmake)

#
# Configure Install
#
include(${PROJECT_SOURCE_DIR}/cmake/InstallConfig.cmake)

