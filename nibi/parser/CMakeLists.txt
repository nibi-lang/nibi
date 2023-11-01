project (parser)

# Add a library with the above sources
add_library(${PROJECT_NAME}
  src/parser.cpp
  src/file_parser.cpp)

add_library(lang::parser ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
