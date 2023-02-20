cmake_minimum_required(VERSION 3.25.2)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


add_executable(ballistic-project)
target_link_libraries(ballistic-project PUBLIC
    project_options
    ballistic-project::engine
)
target_include_directories(ballistic-project PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

target_sources(ballistic-project PRIVATE
  main.cpp
 "Camera.h" "EventManager.h")