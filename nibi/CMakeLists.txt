cmake_minimum_required (VERSION 3.12)

project(nibi)

set(CMAKE_CXX_STANDARD 23)

set(PARSER_SOURCES
  parser/parser.cpp
  parser/file_parser.cpp)

set(APP_SOURCES
  ${PARSER_SOURCES}
  app/main.cpp)

message(STATUS "Parser sources: ${PARSER_SOURCES}")

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME}
  ${APP_SOURCES})
