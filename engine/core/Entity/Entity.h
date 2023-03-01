#pragma once
#include <memory>

#include "../RigidBody.h"
#include "../../utils/Typelist/Typelist.h"
#include "engine/core/PhysicsWorld.h"

class Entity
{
public:
	Entity(): rb(std::make_shared<RigidBody>())
	{
		RegisterEntity();
	}

	virtual ~Entity()
	{
		UnregisterEntity();
	}

	std::shared_ptr<RigidBody> rb;



private:
	void RegisterEntity()
	{
		const auto world = PhysicsWorld::GetWorld();
		world->AddRigidBody(rb);
	}

	void UnregisterEntity() const
	{
		const auto world = PhysicsWorld::GetWorld();
		world->RemoveRigidBody(rb);
	}
};


class SquareEntity;
class CircleEntity;

using AvailableTypes = typelist<SquareEntity, CircleEntity>;