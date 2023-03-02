#pragma once

#include "../../../../engine/Components/PhysicsComponent.h"

class IGameObject;


struct PCBullet : IPhysicsComponent
{
	PCBullet();
	void updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene) override;

};