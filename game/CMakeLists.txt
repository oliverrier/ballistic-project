cmake_minimum_required(VERSION 3.25.2)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


add_executable(ballistic-project)
target_link_libraries(ballistic-project PUBLIC
    project_options
    ballistic-project::engine
    ballistic-project::tools
    ballistic-project::physicsEngine
    sfml-graphics sfml-audio sfml-system
    ballistic-project::thor
)
target_include_directories(ballistic-project PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

file(GLOB_RECURSE GAME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(ballistic-project PRIVATE ${GAME_SOURCES})

# Add all .h files in the directory to the physicsEngine target
file(GLOB_RECURSE GAME_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
target_sources(ballistic-project PRIVATE ${GAME_HEADERS})
