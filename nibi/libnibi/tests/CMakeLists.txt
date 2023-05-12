link_directories(${CPPUTEST_LIBRARIES})

include_directories(
  ${CPPUTEST_INCLUDE_DIRS}
  ../
)

find_package (Threads REQUIRED)

add_executable(lib_nibi_unit_tests
        ${SOURCES}
        input.cpp
        main.cpp)

target_link_libraries(lib_nibi_unit_tests
  ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET lib_nibi_unit_tests COMMAND ./lib_nibi_unit_tests POST_BUILD)