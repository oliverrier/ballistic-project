#pragma once
#include <memory>

#include "../utils/Typelist/Typelist.h"
#include "../utils/Factory/Factory.h"
#include "../../physicsEngine/dynamics/Body.h"

class Entity
{
public:
	Entity() : rb(nullptr), m_world(nullptr)
	{
	}
	Entity(Body* body, World* world) : rb(body), m_world(world)
	{
	}

	virtual ~Entity()
	{
		UnregisterEntity();
	}

	Body* rb;
	World* m_world;
private:
	void RegisterEntity()
	{
		//const auto world = PhysicsWorld::GetWorld();
		//world->AddRigidBody(rb);
	}

	void UnregisterEntity() const
	{
		//const auto world = PhysicsWorld::GetWorld();
		//m_world->RemoveRigidBody(rb);
	}

	// remove for PhysicsWorld::GetWorld();
};


class RectEntity;
class CircleEntity;

using AvailableTypes = typelist<RectEntity, CircleEntity>;
using EntityFactory = Factory<AvailableTypes>;