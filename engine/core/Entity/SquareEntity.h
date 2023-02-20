#pragma once
#include "Entity.h"
#include "../Collider/RectangleCollider.h"

class SquareEntity : public Entity
{
	friend AvailableTypes;
	friend Factory;
private:
	
	SquareEntity() : Entity()
	{
		// set the default collider
		rb->AttachCollider(Rectangle, std::make_unique<RectangleCollider>(rb));
	}

	explicit SquareEntity(FVector2 position, FVector2 size, FVector2 velocity , float mass, bool isStatic = false) : Entity()
	{
		// set the default collider
		rb->position = position;
		rb->velocity = velocity;
		rb->mass = mass;
		rb->SetStatic(isStatic);
		rb->AttachCollider(Rectangle, std::make_unique<RectangleCollider>(rb, size));
	}
public:
	~SquareEntity() override = default;

	void SetSize(const FVector2& size) const
	{
		const auto collider = dynamic_cast<RectangleCollider*>(rb->collider.get());
		collider->size = size;
	}
};
