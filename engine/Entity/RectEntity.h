#include "Entity.h"
#include "physicsEngine/collision/PolygonShape.h"

class RectEntity: public Entity
{
	friend Factory;
private:
	RectEntity(const Vec2& size, const Vec2& position, const BodyType type): size(size)
	{
		BodyDef bd;
		bd.type = type;
		bd.position.Set(position.x, position.y);

		auto world = World::GetWorld();
		Body * body = world->CreateBody(&bd);

		PolygonShape shape;
		shape.SetAsBox(size.x, size.y);

		FixtureDef fd;
		fd.shape = &shape;
		fd.density = 1.0f;
		fd.friction = 0.8f;

		body->CreateFixture(&fd);

		rb = body;
	}
public:
	Vec2 size;
};
