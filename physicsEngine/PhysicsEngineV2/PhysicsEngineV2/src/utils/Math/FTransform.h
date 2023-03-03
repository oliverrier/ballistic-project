#pragma once
#include <cmath>

#include "Vector2.h"


struct FTransform
{
    FVector2 position;
    float Sin;
    float Cos;

    FTransform(FVector2& position, const float angle): position(position), Sin(sinf(angle)), Cos(cosf(angle)) {}

    FTransform(float x, float y, const float angle) : position({x, y}), Sin(sinf(angle)), Cos(cosf(angle)) {}

    static FVector2 Transform(const FVector2& v, const FTransform& transform)
    {
        return {
	        transform.Cos * v.X - transform.Sin * v.Y + transform.position.x,
            transform.Sin * v.X + transform.Cos * v.Y + transform.position.y
        };
    }

};

