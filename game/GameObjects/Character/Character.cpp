#include "game/GameObjects/Character/Character.h"
#include "engine/utils/Math/Common.h"


Character::Character(float window_height)
{
	m_body = EntityFactory::create<RectEntity>(Vec2{ 20.f,  20.f }, Vec2{ 50.f, window_height - 500 }, BodyType::dynamicBody);
	m_boundingBox = new sf::RectangleShape({ m_body->size.x, m_body->size.y });
}

const float Character::getHealth()
{
	return m_health;
}

const float Character::getMaxHealth()
{
	return m_maxHealth;
}

void Character::takeDamage(float damage)
{
	m_health -= damage;
	m_health = Clamp<float>(m_health, 0, m_maxHealth);
}