#ifndef CHARACTER_H
#define CHARACTER_H

#include "game/Components/InputComponents/Character/ICCharacter.h"
#include "game/Components/PhysicsComponents/Character/PCCharacter.h"
#include "game/Components/GraphicsComponents/Character/GCCharacter.h"
#include "engine/GameObject/GameObject.h"
#include "engine/Entity/RectEntity.h"

class Character : public GameObject<ICCharacter, PCCharacter, GCCharacter>, Entity {

public:
	Character(float window_height);
	~Character() override = default;

	void takeDamage(float damage);
	const float getHealth();
	const float getMaxHealth();

	std::shared_ptr<RectEntity> m_body;
	sf::RectangleShape* m_boundingBox;

protected:

	Fixture* m_character;
	

	float m_health;
	float m_maxHealth;

};

#endif // !CHARACTER_H