cmake_minimum_required(VERSION 3.25.2)

add_library(thor)
add_library(ballistic-project::thor ALIAS thor)
target_link_libraries(thor PUBLIC
    sfml-graphics sfml-audio sfml-system
)
target_include_directories(thor PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

file(GLOB_RECURSE THOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(thor PRIVATE ${THOR_SOURCES})

# Add all .h files in the directory to the physicsEngine target
file(GLOB_RECURSE THOR_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
target_sources(thor PRIVATE ${THOR_HEADERS})
