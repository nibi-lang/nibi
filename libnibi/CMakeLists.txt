cmake_minimum_required(VERSION 3.5)

project (libnibi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

option(COMPILE_TESTS   "Execute unit tests" ON)
option(WITH_ASAN       "Compile with ASAN" OFF)

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

if(WITH_ASAN)
  set(CMAKE_CX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

add_library(${PROJECT_NAME} SHARED 
  ${PROJECT_SOURCE_DIR}/src/cell.cpp
  ${PROJECT_SOURCE_DIR}/src/environment.cpp
  ${PROJECT_SOURCE_DIR}/src/source.cpp
)
add_library(nibi::library ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

if(COMPILE_TESTS)
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()