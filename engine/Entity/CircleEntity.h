#pragma once

#include "Entity.h"
#include "physicsEngine/collision/CircleShape.h"
#include "physicsEngine/dynamics/World.h"
#include "physicsEngine/dynamics/Fixture.h"

class CircleEntity: public Entity
{
	friend Factory;
private:
	CircleEntity(float radius, Vec2 position, BodyType type): radius(radius)
	{

		BodyDef bd;
		bd.type = type;
		bd.position.Set(position.x, position.y);

		auto world = World::GetWorld();
		Body* body = world->CreateBody(&bd);

		CircleShape shape;
		shape.m_radius = 0.5f;

		FixtureDef fd;
		fd.shape = &shape;
		fd.density = 1.0f;

		body->CreateFixture(&fd);

		rb = body;
	}
public:
	float radius;
};
