cmake_minimum_required(VERSION 3.25.2)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


add_executable(ballistic-project)
target_link_libraries(ballistic-project PUBLIC
    project_options
    ballistic-project::engine
    ballistic-project::tools
)
target_include_directories(ballistic-project PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

target_sources(ballistic-project PRIVATE
  main.cpp
 "scenes/StartScene.h"
 "scenes/StartScene.cpp"
 "scenes/SceneEnum.h"
 "GameObjects/UI/Button.h"
 "GameObjects/UI/Button.cpp"
 "Components/GraphicsComponents/GCVoid.cpp"
 "Components/GraphicsComponents/GCVoid.h"
 "Components/GraphicsComponents/UI/GCButton.cpp"
 "Components/GraphicsComponents/UI/GCButton.h"
 "Components/InputComponents/ICVoid.cpp"
 "Components/InputComponents/ICVoid.h"
 "Components/InputComponents/UI/ICButton.cpp"
 "Components/InputComponents/UI/ICButton.h"
 "Components/PhysicsComponents/PCVoid.cpp"
 "Components/PhysicsComponents/PCVoid.h"
 "Components/PhysicsComponents/UI/PCButton.cpp"
 "Components/PhysicsComponents/UI/PCButton.h"
 "Camera.h"
 "EventManager.h"
)