#pragma once
#include "Collider.h"
#include "Vector2.h"

class CircleCollider : public Collider
{
public:
    Vector2 center;
    float radius;

    CircleCollider(const Vector2& _center = Vector2(), float _radius = 0.0f) : center(_center), radius(_radius) {}

    bool detectCollision(const Collider* other) const override {
        const auto* circle = dynamic_cast<const CircleCollider*>(other);
        if (circle) {
            const float distanceSquared = (center - circle->center).magnitude() * (center - circle->center).magnitude();
            const float combinedRadius = radius + circle->radius;
            return distanceSquared <= combinedRadius * combinedRadius;
        }
        return false;
    }

    Vector2 getSupportPoint(const Vector2& direction) const override {
	    const Vector2 normal = direction.normalized();
        return center + normal * radius;
    }
};

