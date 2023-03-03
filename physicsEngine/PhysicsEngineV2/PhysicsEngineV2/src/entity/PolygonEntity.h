#pragma once
#include "Entity.h"
#include "../utils/Factory/Factory.h"

class PolygonEntity : public Entity
{
	friend Factory;
private:
	PolygonEntity(const std::vector<FVector2>& vertices, const FVector2& position, const float density, const float restitution, const bool is_static = false) : Entity()
	{
		rb->shapeType = Box;

		rb->Restitution = std::clamp(restitution, 0.f, 1.f);
		rb->IsStatic = is_static;
		rb->Density = density;

		if (!is_static)
		{
			rb->Mass = 5000;
			rb->InvMass = rb->Mass > 0.f ? 1.f / rb->Mass : 0.f;
			rb->Inertia = (1.f / 12) * rb->Mass * 2;
			rb->InvInertia = rb->Inertia > 0.f ? 1.f / rb->Inertia : 0.f;
			rb->InvInertia = 1 / rb->InvInertia;
		}

		rb->vertices = vertices;
		rb->transformedVertices = std::vector<FVector2>(vertices.size());
		rb->position = position;
		rb->transformUpdateRequired = true;
		rb->aabbUpdateRequired = true;
	}
};
