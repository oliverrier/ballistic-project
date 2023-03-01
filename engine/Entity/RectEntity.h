#include "Entity.h"

class RectEntity: public Entity
{
	friend Factory;
private:
	RectEntity(Vec2 size, Vec2 position, BodyType type, World* world): size(size)
	{
		m_world = world;

		BodyDef bd;
		bd.type = type;
		bd.position.Set(position.x, position.y);

		Body* body = m_world->CreateBody(&bd);

		PolygonShape shape;
		shape.SetAsBox(size.x, size.y);

		FixtureDef fd;
		fd.shape = &shape;
		fd.density = 1.0f;
		fd.friction = 0.8f;

		body->CreateFixture(&fd);

		rb = body;
	}

	Vec2 size;
};