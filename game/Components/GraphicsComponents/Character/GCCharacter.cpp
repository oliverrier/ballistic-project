#include "GCCharacter.h"

#include "game/GameObjects/Character/Character.h"


GCCharacter::GCCharacter()
{
}

void GCCharacter::renderImplementation(IGameObject& gameObject, sf::RenderWindow& window)
{
	Character& character = static_cast<Character&>(gameObject);

	character.m_boundingBox->setOrigin(character.m_boundingBox->getSize() / 2.f);
	window.draw(*character.m_boundingBox);
}
