cmake_minimum_required (VERSION 3.8)

project(nibi)

set(CMAKE_CXX_STANDARD 20)

#
# Options
#
option(COMPILE_TESTS "Execute unit tests" ON)
option(WITH_ASAN     "Compile with ASAN" OFF)

#
# Setup build type 'Release vs Debug'
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

#
# Locate CPPUTest
#
if(DEFINED ENV{CPPUTEST_HOME})
    message(STATUS "Using CppUTest home: $ENV{CPPUTEST_HOME}")
    set(CPPUTEST_INCLUDE_DIRS $ENV{CPPUTEST_HOME}/include)
    set(CPPUTEST_LIBRARIES $ENV{CPPUTEST_HOME}/lib)
    set(CPPUTEST_LDFLAGS CppUTest CppUTestExt)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(CPPUTEST REQUIRED cpputest>=3.8)
    message(STATUS "Found CppUTest version ${CPPUTEST_VERSION}")
endif()

#
# Setup ASAN
#
if(WITH_ASAN)
  set(CMAKE_CX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

add_subdirectory(libnibi)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(APP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/builtins.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/lambdas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/arithmetic.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/environment_modifiers.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/asserts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/debug.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/at_commands.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/list_commands.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/bitwise.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/comparison.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/common.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/env_cell.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/builtins/excepts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/error.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/input.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/list.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/token.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/interpreter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/app/app.cpp
)

add_executable(${PROJECT_NAME}
        ${APP_SOURCES})

target_link_libraries(${PROJECT_NAME}
  PUBLIC  
  libnibi)

#
# Tests
#
if(COMPILE_TESTS)
  add_subdirectory(tests)
endif()