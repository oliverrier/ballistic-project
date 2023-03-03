#pragma once

#include "Collider.h"

constexpr float INF = std::numeric_limits<float>::infinity();

class Collision {
public:
	static Collision Instance;

	bool SATCollision(const Collider& body, const Collider& other, FVector2& MTV);

	bool SATCollision(const CircleCollider& body, const Collider& other, FVector2& MTV);
	bool SATCollision(const Collider& body, const CircleCollider& other, FVector2& MTV);

	bool SATCollision(const CircleCollider& body, const CircleCollider& other, FVector2& MTV);

private:
	Collision();
	~Collision();

	FVector2 CircleAxis(std::vector<FVector2>& vertices, uint32_t count, FVector2 center);

	FVector2 PerpendicularAxis(std::vector<FVector2>& vertices, uint32_t index, uint32_t count) const;

	FVector2 ProjectOnto(std::vector<FVector2>& vertices, uint32_t count, FVector2 axis) const;
	FVector2 ProjectCircle(FVector2 circleCenter, float radius, FVector2 axis) const;

	FVector2 GetCenter(const Collider& body) const;

	float Overlap(FVector2 v0, FVector2 v1) const;
};