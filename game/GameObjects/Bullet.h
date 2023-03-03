#pragma once

#include "engine/GameObject/GameObject.h"
#include "game/Components/InputComponents/Bullets/ICBullet.h"
#include "game/Components/PhysicsComponents/Bullets/PCBullet.h"
#include "game/Components/GraphicsComponents/Bullets/GCBullet.h"
#include "engine/Entity/CircleEntity.h"

struct Bullet : GameObject<GCBullet, PCBullet, ICBullet>
{
	Bullet(float angle, Vec2 position, bool isFragmentation = false);
	~Bullet() = default;

	std::shared_ptr<CircleEntity> m_body;
	sf::CircleShape m_circle;
	float angle;
	bool is_fragmentation;
};