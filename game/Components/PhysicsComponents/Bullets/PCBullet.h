#pragma once

#include "../../../../engine/Components/PhysicsComponent.h"
#include "physicsEngine/common/Math.h"

class IGameObject;


struct PCBullet : IPhysicsComponent
{
	PCBullet();
	void updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene) override;
	void ApplyDamage(IScene& game_scene, Vec2 bulletPos, bool isFrag);

};