cmake_minimum_required(VERSION 3.25.2)

add_library(physicsEngine)
add_library(ballistic-project::physicsEngine ALIAS physicsEngine)
target_link_libraries(physicsEngine PUBLIC
    project_options
)
target_include_directories(physicsEngine PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

file(GLOB_RECURSE PHYSICSENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(physicsEngine PRIVATE ${PHYSICSENGINE_SOURCES})

# Add all .h files in the directory to the physicsEngine target
file(GLOB_RECURSE PHYSICSENGINE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
target_sources(physicsEngine PRIVATE ${PHYSICSENGINE_HEADERS})
