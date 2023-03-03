#pragma once
#include "Collider.h"
#include "Vector2.h"

class RigidBody {
public:
    float mass;
    float invMass;
    float inertia;
    float invInertia;
    Vector2 position;
    Vector2 velocity;
    float angularVelocity;
    float torque;
    float angle;
    Collider* collider;

    RigidBody(const float mass, const float inertia, Collider* collider) : mass(mass), invMass(1.0f / mass), inertia(inertia), invInertia(1.0f / inertia), collider(collider) {}

    void applyForce(const Vector2& force) {
        velocity += force * invMass;
    }

    void applyTorque(const float torque) {
        this->torque += torque;
    }

    void update(float deltaTime) {
	    const Vector2 acceleration = velocity * -0.1f;
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;

	    const float angularAcceleration = torque * invInertia * -0.1f;
        angularVelocity += angularAcceleration * deltaTime;
        angle += angularVelocity * deltaTime;
        torque = 0.0f;
    }
};
