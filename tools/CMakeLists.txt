cmake_minimum_required(VERSION 3.25.2)


add_library(tools)
add_library(ballistic-project::tools ALIAS tools)
target_link_libraries(tools PUBLIC
    project_options
)
target_include_directories(tools PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

target_sources(tools PRIVATE
  "MathUtils.h"
  "MathUtils.cpp"
  "DesignPatterns/Composite.h"
  "DesignPatterns/Singleton.h"
)