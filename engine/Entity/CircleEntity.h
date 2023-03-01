#include "Entity.h"

class CircleEntity: public Entity
{
	friend Factory;
private:
	CircleEntity(float radius, Vec2 position, BodyType type, World* world): radius(radius)
	{
		m_world = world;

		BodyDef bd;
		bd.type = type;
		bd.position.Set(position.x, position.y);

		Body* body = m_world->CreateBody(&bd);

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