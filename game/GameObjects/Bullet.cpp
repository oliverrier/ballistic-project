#include "Bullet.h"

#include <iostream>

Bullet::Bullet(float window_width, float* angle) : m_angle(angle){
	m_body = EntityFactory::create<CircleEntity>(5.f, Vec2{ window_width / 2.f, 10.0f }, BodyType::dynamicBody);
	m_circle = new sf::CircleShape(m_body->radius);
}