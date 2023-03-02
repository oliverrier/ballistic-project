cmake_minimum_required(VERSION 3.25.2)

add_library(engine)
add_library(ballistic-project::engine ALIAS engine)
file(COPY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../_deps/sfml-src/extlibs/bin/x64/openal32.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_link_libraries(engine PUBLIC
    project_options
    ballistic-project::tools
    sfml-graphics sfml-audio sfml-system
    ballistic-project::physicsEngine
    ballistic-project::thor
)
target_include_directories(engine PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/Assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE E_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(engine PRIVATE ${E_SOURCES})

# Add all .h files in the directory to the physicsEngine target
file(GLOB_RECURSE E_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
target_sources(engine PRIVATE ${E_HEADERS})
