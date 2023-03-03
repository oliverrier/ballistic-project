#pragma once

#include "CircleCollider.h"

class RectangleCollider : public Collider {
    DECLARE_RTTI(RectangleCollider, Collider)
public:

    RectangleCollider(const std::shared_ptr<RigidBody> rb, const FVector2& size) : rb(rb), size(size) {}
    RectangleCollider(const std::shared_ptr<RigidBody> rb) : rb(rb), size({}) {}

    std::shared_ptr<RigidBody> rb;
    FVector2 size;
        
    inline FVector2 GetHalfExtents() const
    {
        return { size.x / 2, size.y / 2 };
    }

    inline FVector2 GetHalfSize() const
    {
        return size / 2.0;
    }

    bool IsPointInRectangle(const FVector2& point) const
    {
	    const FVector2 min = rb->position - GetHalfExtents();
	    const FVector2 max = rb->position + GetHalfExtents();
        return (point.x >= min.x && point.x <= max.x) && (point.y >= min.y && point.y <= max.y);
    }

    bool IsSegmentIntersectingRectangle(const FVector2& a, const FVector2& b) const
    {
	    const FVector2 halfExtents = GetHalfExtents();

        // Translate the rectangle so that the segment is relative to it
        FVector2 d = b - a;
        FVector2 e = rb->position - a;
        FVector2 f = e.Perpendicular();
    	f.Normalize();

        // Check if the segment intersects the top or bottom side of the rectangle
        if (abs(f.x) > halfExtents.x || abs(f.y) > halfExtents.y)
            return false;

        // Check if the segment intersects the left or right side of the rectangle
        float t1 = e.Perpendicular().Dot(halfExtents) / f.Dot(halfExtents);
        float t2 = (e + d).Perpendicular().Dot(halfExtents) / f.Dot(halfExtents);
        return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
    }
};
