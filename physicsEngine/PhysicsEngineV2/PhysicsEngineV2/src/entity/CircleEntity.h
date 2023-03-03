#pragma once
#include "Entity.h"
#include "../utils/Factory/Factory.h"

class CircleEntity : public Entity
{
	friend Factory;
private:
	CircleEntity(const float radius, const FVector2& position, const float density, const float restitution, const bool is_static = false) : Entity()
	{
		rb->shapeType = Circle;
		rb->Radius = radius;
		rb->Area = radius * radius * PI;
		rb->Restitution = std::clamp(restitution, 0.f, 1.f);
		rb->IsStatic = is_static;

		if (!is_static)
		{
			rb->Mass = rb->Area * density;
			rb->InvMass = 1 / rb->Mass;
			rb->Inertia = (1.f / 2.f) * rb->Mass * radius * radius;
			rb->InvInertia = 1 / rb->InvInertia;

		}

		rb->position = position;
	}
};
