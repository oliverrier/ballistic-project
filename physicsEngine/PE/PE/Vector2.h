#pragma once

#include <cmath>

class Vector2 {
public:
    float x, y;

    Vector2(const float _x = 0.0f, const float _y = 0.0f) : x(_x), y(_y) {}

    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2 normalized() const {
        const float mag = magnitude();
        if (mag > 0) {
            return {x / mag, y / mag};
        }
        return {};
    }

    Vector2 operator+ (const Vector2& other) const {
        return {x + other.x, y + other.y};
    }

    Vector2 operator- (const Vector2& other) const {
        return {x - other.x, y - other.y};
    }

    Vector2 operator* (const float scalar) const {
        return {x * scalar, y * scalar};
    }

    Vector2 operator/ (const float scalar) const {
        return {x / scalar, y / scalar};
    }

    void operator+= (const Vector2& other) {
        x += other.x;
        y += other.y;
    }

    void operator-= (const Vector2& other) {
        x -= other.x;
        y -= other.y;
    }

    void operator*= (const float scalar) {
        x *= scalar;
        y *= scalar;
    }

    void operator/= (const float scalar) {
        x /= scalar;
        y /= scalar;
    }
};