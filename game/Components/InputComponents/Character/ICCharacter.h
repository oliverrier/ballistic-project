#ifndef ICCHARACTER_H
#define ICCHARACTER_H

#include "../../../../Engine/Components/InputComponent.h"

class IGameObject;


struct ICCharacter : IInputComponent
{
	ICCharacter();
	void processInputImplementation(IGameObject& gameObject, sf::Event& inputEvent, IScene& scene) override;
};

#endif // ICCHARACTER_H