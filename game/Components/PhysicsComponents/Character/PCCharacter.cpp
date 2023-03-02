#include "PCCharacter.h"

#include "game/GameObjects/Character/Character.h"

PCCharacter::PCCharacter() : m_callbackIsCalled(false)
{
}

void PCCharacter::updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene)
{
	Character& character = static_cast<Character&>(gameObject);

	character.m_boundingBox->setPosition({ character.m_body->rb->GetPosition().x, character.m_body->rb->GetPosition().y });
}
