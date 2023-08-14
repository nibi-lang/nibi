
cmake_minimum_required (VERSION 3.8)

project(nibi)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libffi REQUIRED libffi)

#
# Setup build type 'Release vs Debug'
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()


if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DDEBUG_BUILD)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/CheckGit.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(APP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/nibi.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/source.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/intake/token.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/intake/input_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/intake/parser.cpp
)

add_executable(${PROJECT_NAME}
        ${APP_SOURCES})

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    ffi
    ${CMAKE_DL_LIBS})

install (TARGETS ${PROJECT_NAME}
    DESTINATION bin)
