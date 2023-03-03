#include "Bullet.h"

#include <iostream>

#include "engine/Game/Game.h"
#include "engine/Scene/Scene.h"

Bullet::Bullet(float angle, Vec2 position, bool isFragmentation) : angle(angle), is_fragmentation(isFragmentation){
	float r = isFragmentation ? 2.f : 5.f;
	m_body = EntityFactory::create<CircleEntity>(r, position, BodyType::dynamicBody);
	m_circle = sf::CircleShape(m_body->radius);
}
