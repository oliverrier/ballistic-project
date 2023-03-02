#include "game/GameObjects/Character/Character.h"
#include "engine/utils/Math/Common.h"

Character::Character(std::shared_ptr<RectEntity> body, sf::RectangleShape* boundingBox) : m_body(body), m_boundingBox(boundingBox)
{
	
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