#include "Entity.h"
#include "physicsEngine/collision/CircleShape.h"

class CircleEntity: public Entity
{
	friend Factory;
private:
	CircleEntity(float radius, Vec2 position, BodyType type): radius(radius)
	{

		BodyDef bd;
		bd.type = type;
		bd.position.Set(position.x, position.y);
		bd.allowSleep = false;

		auto world = World::GetWorld();
		Body* body = world->CreateBody(&bd);

		CircleShape shape;
		shape.m_radius = radius;

		FixtureDef fd;
		fd.shape = &shape;
		fd.density = 1.0f;
		fd.friction = 1.0f;
		fd.restitution = 0.4f;

		body->CreateFixture(&fd);

		rb = body;
	}
public:
	float radius;
};
