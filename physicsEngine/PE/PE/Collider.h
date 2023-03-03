#pragma once


class Collider {
public:
    virtual bool detectCollision(const Collider* other) const = 0;
    virtual Vector2 getSupportPoint(const Vector2& direction) const = 0;
};
