link_directories(${CPPUTEST_LIBRARIES})

include_directories(
  ${CPPUTEST_INCLUDE_DIRS}
  ../
)

find_package (Threads REQUIRED)

add_executable(unit_tests
        ${COMPILER_SOURCES}
        nibi_stub.cpp
        main.cpp)

target_link_libraries(unit_tests
  ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET unit_tests COMMAND ./unit_tests POST_BUILD)