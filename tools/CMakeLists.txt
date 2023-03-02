cmake_minimum_required(VERSION 3.25.2)


add_library(tools)
add_library(ballistic-project::tools ALIAS tools)
target_link_libraries(tools PUBLIC
    project_options
)
target_include_directories(tools PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

file(GLOB_RECURSE T_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(tools PRIVATE ${T_SOURCES})

# Add all .h files in the directory to the physicsEngine target
file(GLOB_RECURSE T_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
target_sources(tools PRIVATE ${T_HEADERS})
