cmake_minimum_required(VERSION 3.25.2)

add_library(engine)
add_library(ballistic-project::engine ALIAS engine)
target_link_libraries(engine PUBLIC
    project_options
    sfml-graphics sfml-audio sfml-system
)
target_include_directories(engine PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

target_sources(engine PRIVATE
  test.cpp
  test.h
)