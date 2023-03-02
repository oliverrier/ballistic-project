#include "Entity.h"
#include "physicsEngine/collision/ChainShape.h"

class PolygonEntity : public Entity
{
	friend Factory;
private:
	PolygonEntity(const std::vector<Vec2>& vertices, Vec2 position, BodyType type) : vertices(vertices)
	{

		BodyDef bd;
		bd.type = type;
		bd.position.Set(position.x, position.y);

		auto world = World::GetWorld();
		Body* body = world->CreateBody(&bd);

		ChainShape shape;
		shape.CreateLoop(vertices, vertices.size());

		body->CreateFixture(&shape, 0.f);

		rb = body;
	}
public:
	std::vector<Vec2> vertices;
};
