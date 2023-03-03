#pragma once
#include "Collider.h"
#include "Vector2.h"


class RectangleCollider : public Collider {
public:
    Vector2 center;
    Vector2 halfExtents;

    RectangleCollider(const Vector2& _center = Vector2(), const Vector2& _halfExtents = Vector2()) : center(_center), halfExtents(_halfExtents) {}

    bool detectCollision(const Collider* other) const override {
        const auto* rect = dynamic_cast<const RectangleCollider*>(other);
        if (rect) {
            const Vector2 minA = center - halfExtents;
            const Vector2 maxA = center + halfExtents;
            const Vector2 minB = rect->center - rect->halfExtents;
            const Vector2 maxB = rect->center + rect->halfExtents;
            return (minA.x <= maxB.x && maxA.x >= minB.x && minA.y <= maxB.y && maxA.y >= minB.y);
        }
        return false;
    }

    Vector2 getSupportPoint(const Vector2& direction) const override {
        const Vector2 normal = direction.normalized();
        Vector2 point = center + Vector2(halfExtents.x * normal.x, halfExtents.y * normal.y);
        if (normal.x < 0) {
            point.x -= halfExtents.x * 2.0f;
        }
        if (normal.y < 0) {
            point.y -= halfExtents.y * 2.0f;
        }
        return point;
    }
};
