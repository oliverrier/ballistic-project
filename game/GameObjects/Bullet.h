#pragma once

#include "engine/GameObject/GameObject.h"
#include "game/Components/InputComponents/Bullets/ICBullet.h"
#include "game/Components/PhysicsComponents/Bullets/PCBullet.h"
#include "game/Components/GraphicsComponents/Bullets/GCBullet.h"
#include "engine/Entity/CircleEntity.h"

struct Bullet : public GameObject<GCBullet, PCBullet, ICBullet>
{
public:
	Bullet(std::shared_ptr<CircleEntity> body, sf::CircleShape* circle, float angle);
	~Bullet() = default;


	std::shared_ptr<CircleEntity> m_body;
	sf::CircleShape* m_circle;
	float angle;
	
};