#pragma once

#include <memory>

#include "engine/Entity/RectEntity.h"
#include "engine/GameObject/GameObject.h"
#include "game/Components/GraphicsComponents/Wall/GCWall.h"
#include "game/Components/InputComponents/ICVoid.h"
#include "game/Components/PhysicsComponents/PCvoid.h"
#include "physicsEngine/common/Math.h"


struct Wall : GameObject<GCWall, PCVoid, ICVoid>
{
	Wall(Vec2& size, Vec2& position);
	~Wall() override = default;


	std::shared_ptr<RectEntity> m_body;
	sf::RectangleShape m_shape;
};
