#pragma once

#include "engine/GameObject/GameObject.h"
#include "game/Components/InputComponents/Bullets/ICBullet.h"
#include "game/Components/PhysicsComponents/Bullets/PCBullet.h"
#include "game/Components/GraphicsComponents/Bullets/GCBullet.h"
#include "engine/Entity/CircleEntity.h"

struct Bullet : public GameObject<GCBullet, PCBullet, ICBullet>
{
public:
	Bullet(float window_width, float* angle);
	~Bullet() = default;


	std::shared_ptr<CircleEntity> m_body;
	sf::CircleShape* m_circle;
	float* m_angle;
	bool m_destroyed = true;
	
};