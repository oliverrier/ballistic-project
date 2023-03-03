#pragma once
#include "Entity.h"
#include "../Collider/CircleCollider.h"
#include "../../utils/Factory/Factory.h"

class CircleEntity : public Entity
{
	friend Factory;
private:

	CircleEntity() : Entity()
	{
		// set the default collider
		rb->AttachCollider(Circle, std::make_unique<CircleCollider>(rb, 0.f));
	}

	explicit CircleEntity(FVector2 position, float radius, FVector2 velocity, float mass, bool isStatic = false) : Entity()
	{
		// set the default collider
		rb->position = position;
		rb->velocity = velocity;
		rb->mass = mass;
		rb->SetStatic(isStatic);
		rb->AttachCollider(Circle, std::make_unique<CircleCollider>(rb, radius));
	}
public:
	~CircleEntity() override = default;

	void SetRadius(const float& radius) const
	{
		const auto collider = rb->collider->As<CircleCollider>();
		collider->radius = radius;
	}
};
