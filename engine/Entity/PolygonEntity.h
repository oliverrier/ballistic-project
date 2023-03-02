#include "Entity.h"
#include "physicsEngine/collision/ChainShape.h"
#include "physicsEngine/collision/EdgeShape.h"

class PolygonEntity : public Entity
{
	friend Factory;
private:
	PolygonEntity(const std::vector<Vec2>& vertices, Vec2 position) : vertices(vertices)
	{

		BodyDef bd;
		bd.type = staticBody;
		bd.position.Set(position.x, position.y);

		auto world = World::GetWorld();
		Body* body = world->CreateBody(&bd);

		EdgeShape shape;

		for (int i = 0; i < vertices.size(); ++i)
		{
			auto i1 = i;
			auto i2 = i < vertices.size() - 1 ? i + 1 : 0;
			auto v1 = vertices[i1];
			auto v2 = vertices[i2];
			FixtureDef fdef;
			EdgeShape shape;
			shape.SetTwoSided(v1, v2);
			fdef.shape = &shape;
			fdef.friction = 1;
			body->CreateFixture(&fdef);
		}

		rb = body;
	}
public:
	std::vector<Vec2> vertices;
};
