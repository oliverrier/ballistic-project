#include "Bullet.h"

#include <iostream>

#include "engine/Game/Game.h"
#include "engine/Scene/Scene.h"

Bullet::Bullet(float window_width, float angle) : angle(angle){
	m_body = EntityFactory::create<CircleEntity>(5.f, Vec2{ window_width / 2.f, 10.0f }, BodyType::dynamicBody);
	m_circle = sf::CircleShape(m_body->radius);
}
