cmake_minimum_required (VERSION 3.12)

project(nibi-language)

set(CMAKE_CXX_STANDARD 23)

find_package(fmt)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${FMT_INCLUDE_DIR})

# Language sources

set(NIBI_SOURCES
  lang/input/reader.cpp
  lang/input/atomiser.cpp)

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

