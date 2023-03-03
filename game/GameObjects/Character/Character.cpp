#include "game/GameObjects/Character/Character.h"

constexpr int window_height = 1080;


Character::Character(Vec2 pos, sf::Keyboard::Key left, sf::Keyboard::Key right, int index): left(left), right(right), index(index)
{
	m_body = EntityFactory::create<RectEntity>(Vec2{ 20.f,  20.f }, pos, BodyType::dynamicBody);
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