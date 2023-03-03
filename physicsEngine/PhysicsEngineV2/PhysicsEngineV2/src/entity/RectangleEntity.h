#pragma once
#include "Entity.h"
#include "../utils/Factory/Factory.h"

class RectangleEntity : public Entity
{
	friend Factory;
private:
	RectangleEntity(const FVector2& size, const FVector2& position, const float density, const float restitution, const bool is_static = false) : Entity()
	{
		rb->Width = size.x;
		rb->Height = size.y;
		rb->shapeType = Box;

		rb->Area = size.x * size.y;
		rb->Restitution = std::clamp(restitution, 0.f, 1.f);
		rb->IsStatic = is_static;
		rb->Density = density;

		if (!is_static)
		{
			rb->Mass = rb->Area * density;
			rb->InvMass = rb->Mass > 0.f ? 1.f / rb->Mass : 0.f;
			rb->Inertia = (1.f / 12) * rb->Mass * (size.x * size.x + size.y * size.y);
			rb->InvInertia = rb->Inertia > 0.f ? 1.f / rb->Inertia : 0.f;
			rb->InvInertia = 1 / rb->InvInertia;
		}

		rb->vertices = RigidBody::CreateBoxVertices(size.x, size.y);
		rb->transformedVertices = std::vector<FVector2>(4);
		rb->position = position;
		rb->transformUpdateRequired = true;
		rb->aabbUpdateRequired = true;
	}
};
