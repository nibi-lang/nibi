cmake_minimum_required (VERSION 3.8)

project(nibi)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(libffi REQUIRED libffi)

#
# Options
#
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
# Setup ASAN
#
if(WITH_ASAN)
  set(CMAKE_CX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

include(${PROJECT_SOURCE_DIR}/cmake/CheckGit.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/FindFFI.cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/app)
include_directories(${LIBFFI_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(APP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/app/app.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/app/repl/repl.cpp
)


add_executable(${PROJECT_NAME}
        ${APP_SOURCES})

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    ${LIBFFI_LIBRARY}
    libnibi
    Threads::Threads
    ${CMAKE_DL_LIBS})

if (APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_RPATH "/usr/local/lib"
    INSTALL_RPATH "/usr/local/lib"
    )
endif()

install (TARGETS ${PROJECT_NAME}
    DESTINATION bin)
