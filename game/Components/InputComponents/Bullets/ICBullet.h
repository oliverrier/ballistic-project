#pragma once

#include "../../../../engine/Components/InputComponent.h"

class IGameObject;


struct ICBullet : IInputComponent
{
	ICBullet();
	void processInputImplementation(IGameObject& gameObject, sf::Event& inputEvent, IScene& scene) override;
};