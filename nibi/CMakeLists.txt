cmake_minimum_required (VERSION 3.12)

project(nibi-language)

set(CMAKE_CXX_STANDARD 23)

find_package(fmt)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${FMT_INCLUDE_DIR})

# Language sources

set(INTAKE_SOURCES
  front/intake.cpp
  front/atomiser.cpp
  front/generator.cpp
  front/intake.cpp
  front/tracer.cpp
  front/builtins.cpp
  front/imports.cpp)

set(MACHINE_SOURCES
  machine/instructions.cpp
  machine/engine.cpp
  machine/object.cpp)

set(CPPMOD_SOURCES
  cppmod/modules.cpp
  cppmod/io/io.cpp)

set(NIBI_SOURCES
  ${INTAKE_SOURCES}
  ${MACHINE_SOURCES}
  ${CPPMOD_SOURCES})

# Make into lib for quicker compile time

add_library(nibi_static STATIC ${NIBI_SOURCES})

# Build the main app

set(MAIN_APP_NAME "nibi")
set(MAIN_APP_SOURCES
  apps/nibi/main.cpp)

add_executable(${MAIN_APP_NAME}
  ${MAIN_APP_SOURCES})

target_link_libraries(
  ${MAIN_APP_NAME}
  nibi_static
  fmt::fmt)

# Build the debugger

set(DEBUGGER_APP_NAME "ndb")
set(DEBUGGER_APP_SOURCES
  apps/ndb/ndb.cpp
  apps/ndb/main.cpp)

add_executable(${DEBUGGER_APP_NAME}
  ${DEBUGGER_APP_SOURCES})

target_link_libraries(
  ${DEBUGGER_APP_NAME}
  nibi_static
  fmt::fmt)
