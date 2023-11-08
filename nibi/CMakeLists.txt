cmake_minimum_required (VERSION 3.12)

project(nibi)

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

set(APP_SOURCES
  ${INTAKE_SOURCES}
  ${MACHINE_SOURCES}
  app/main.cpp)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${FMT_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
  ${APP_SOURCES})

target_link_libraries(
  ${PROJECT_NAME}
  fmt::fmt)
