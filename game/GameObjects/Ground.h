#pragma once

#include <memory>
#include <vector>

#include "engine/GameObject/GameObject.h"
#include "game/Components/GraphicsComponents/Ground/GCGround.h"
#include "engine/Entity/PolygonEntity.h"
#include "game/Components/InputComponents/ICVoid.h"
#include "game/Components/PhysicsComponents/PCvoid.h"
#include "lib/thor/include/Thor/Shapes/ConcaveShape.hpp"
#include "physicsEngine/common/Math.h"


struct Ground : GameObject<GCGround, PCVoid, ICVoid>
{
	Ground(std::vector<Vec2>& vertices, Vec2& position);
	~Ground() override = default;


	std::shared_ptr<PolygonEntity> m_body;
	thor::ConcaveShape m_shape;
};
