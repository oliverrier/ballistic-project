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

target_sources(engine PRIVATE
 "Scene/Scene.cpp"
 "Scene/Scene.h"
 "GameObject/GameObject.h"
 "Components/GraphicsComponent.h"
 "Components/InputComponent.h"
 "Components/PhysicsComponent.h"
 "Game/Game.h"
 "Game/Game.cpp"
 "GameObject/GameObject.h"
 "Ui/UiFactory.h"
 "utils/Factory/Factory.h"
 "utils/Math/Common.h"
 "utils/Math/Quat2D.h"
 "utils/Math/Vector.h"
 "utils/Math/Vector2.h"
 "utils/RTTI/RTTI.h"
 "utils/Typelist/Typelist.h"
 "Ui/HUD/HudElement.h"   
 "Ui/HUD/HudArrow.h" 
 "Entity/RectEntity.h" 
 "Entity/Entity.h" 
 "Entity/CircleEntity.h" 
 "Entity/PolygonEntity.h" 
 )