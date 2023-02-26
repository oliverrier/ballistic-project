cmake_minimum_required(VERSION 3.25.2)

add_library(engine)
add_library(ballistic-project::engine ALIAS engine)
file(COPY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../_deps/sfml-src/extlibs/bin/x64/openal32.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_link_libraries(engine PUBLIC
    project_options
    ballistic-project::tools
    sfml-graphics sfml-audio sfml-system
)
target_include_directories(engine PUBLIC
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
)

target_sources(engine PRIVATE
  "test.cpp"
  "test.h"
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
 "core/Collider/CircleCollider.h"
 "core/Collider/Collider.h"
 "core/Collider/PolygonCollider.h"
 "core/Collider/RectangleCollider.h"
 "core/Entity/Entity.h"
 "core/Entity/CircleEntity.h"
 "core/Entity/EntityFactory.h"
 "core/Entity/SquareEntity.h"
 "core/CollisionResolver.h"
 "core/RigidBody.h"
 "core/PhysicsWorld.h"
 "core/Transform.h"
 "utils/Factory/Factory.h"
 "utils/Math/Common.h"
 "utils/Math/Quat2D.h"
 "utils/Math/Vector.h"
 "utils/Math/Vector2.h"
 "utils/RTTI/RTTI.h"
 "utils/Typelist/Typelist.h")