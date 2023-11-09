cmake_minimum_required (VERSION 3.12)

project(nibi-language)

set(CMAKE_CXX_STANDARD 23)

find_package(fmt)

set(INTAKE_SOURCES
  front/intake.cpp
  front/lexer.cpp
  front/parser.cpp
  front/intake.cpp
  front/tracer.cpp
  front/builtins.cpp)

set(MACHINE_SOURCES
  machine/instructions.cpp
  machine/engine.cpp
  machine/object.cpp)

set(NIBI_SOURCES
  ${INTAKE_SOURCES}
  ${MACHINE_SOURCES})

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${FMT_INCLUDE_DIR})

# Build the main app

set(MAIN_APP_NAME "nibi")
set(MAIN_APP_SOURCES
  ${NIBI_SOURCES}
  apps/nibi/main.cpp)

add_executable(${MAIN_APP_NAME}
  ${MAIN_APP_SOURCES})

target_link_libraries(
  ${MAIN_APP_NAME}
  fmt::fmt)

# Build the debugger

set(DEBUGGER_APP_NAME "ndb")
set(DEBUGGER_APP_SOURCES
  ${NIBI_SOURCES}
  apps/ndb/main.cpp)

add_executable(${DEBUGGER_APP_NAME}
  ${DEBUGGER_APP_SOURCES})

target_link_libraries(
  ${DEBUGGER_APP_NAME}
  fmt::fmt)
