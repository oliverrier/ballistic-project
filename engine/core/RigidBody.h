#pragma once
#include "../utils/Math/Vector2.h"

class Collider;

enum ColliderType
{
    None = 0,
    Rectangle = 1,
    Circle = 2,
};

class RigidBody {
public:
    RigidBody(): position({}), velocity({}), mass(0.000001f), force({}), colliderType(None) {}
    RigidBody(const FVector2& position, const FVector2& velocity, const float mass) :
        position(position), velocity(velocity), mass(mass), force(FVector2::Zero()), colliderType(None)
    {
    }

    void AddForce(const FVector2& force)
    {
        this->force += force;
    }

    void Update(const float deltaTime)
    {
        // Apply the force to update the velocity
        velocity += force / mass * deltaTime;

        // Update the position based on the velocity
        position += velocity * deltaTime;

        // Reset the force for the next update
        force = FVector2::Zero();
    }

    void AttachCollider(const ColliderType type, std::unique_ptr<Collider> collider)
    {
        this->collider = std::move(collider);
        this->colliderType = type;
    }

    void SetStatic(bool isStatic)
    {
        this->isStatic = isStatic;
    }



    FVector2 position;
    FVector2 velocity;
    float mass;
    FVector2 force;
    ColliderType colliderType;
    bool isStatic = false;

    std::unique_ptr<Collider> collider;

private:

    inline float GetMassInv() const {
    	return 1 / mass;
    }


    void SetColliderType(const ColliderType& type)
    {
        colliderType = type;
    }
};


