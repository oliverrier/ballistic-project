#pragma once
#include "../utils/Math/Vector2.h"

struct AABB
{
	AABB() : min({}), max({}) {}
	AABB(const FVector2& min, const FVector2& max) : min(min), max(max) {}
	AABB(const float min_x, const float min_y, const float max_x, const float max_y) : min({ min_x, min_y }), max({ max_x, max_y }) {}

	FVector2 min;
	FVector2 max;
};