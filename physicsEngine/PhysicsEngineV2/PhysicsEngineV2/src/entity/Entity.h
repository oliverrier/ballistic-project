#pragma once
#include <memory>
#include "../core/PhysicsWorld.h"
#include "../core/RigidBody.h"

class Entity
{
public:
	Entity() : rb(std::make_shared<RigidBody>())
	{
		RegisterEntity();
	}

	virtual ~Entity()
	{
		UnregisterEntity();
	}

	std::shared_ptr<RigidBody> rb;

private:
	void RegisterEntity() const
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
