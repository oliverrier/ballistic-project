#pragma once

#include "../utils/Typelist/Typelist.h"
#include "../utils/Factory/Factory.h"
#include "../../physicsEngine/dynamics/Body.h"

class Entity
{
public:
	Entity() : rb(nullptr)
	{
	}
	Entity(Body* body) : rb(body)
	{
	}

	virtual ~Entity()
	{
		UnregisterEntity();
	}

	Body* rb;
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
class PolygonEntity;

using AvailableTypes = typelist<RectEntity, CircleEntity, PolygonEntity>;
using EntityFactory = Factory<AvailableTypes>;