#include "Collision.h"

#pragma warning(disable : 6386)

Collision Collision::Instance;

bool Collision::SATCollision(const Collider& body, const Collider& other, FVector2& MTV) {
	float minOverlap = INF;

	std::vector<FVector2> bodyVertices = body.m_vertices;
	std::vector<FVector2> otherVertices = other.m_vertices;

	uint32_t bodyCount = body.m_verticesCount;
	uint32_t otherCount = other.m_verticesCount;

	uint32_t all = bodyCount + otherCount;

	if (all == 2)
		return SATCollision((const CircleCollider&)body, (const CircleCollider&)other, MTV);
	else if (bodyCount == 1)
		return SATCollision((const CircleCollider&)body, other, MTV);
	else if (otherCount == 1)
		return SATCollision(body, (const CircleCollider&)other, MTV);

	std::vector<FVector2> axis = std::vector<FVector2>(all);

	for (uint32_t i = 0; i < bodyCount; i++)
		axis[i] = PerpendicularAxis(bodyVertices, i, bodyCount);

	for (uint32_t i = 0; i < otherCount; i++)
		axis[i + bodyCount] = PerpendicularAxis(otherVertices, i, otherCount);

	for (uint32_t i = 0; i < all; i++) {
		auto& a = axis[i];

		FVector2 bodyProjection = ProjectOnto(bodyVertices, bodyCount, a);
		FVector2 otherProjection = ProjectOnto(otherVertices, otherCount, a);

		float overlap = Overlap(bodyProjection, otherProjection);

		if (!overlap) {
			MTV = FVector2(0.0f, 0.0f);

			return false;
		}
		else {
			if (overlap < minOverlap) {
				minOverlap = overlap;

				MTV = a * overlap;
			}
		}
	}

	if ((GetCenter(body) - GetCenter(other)).Dot(MTV) < 0.0f)
		MTV *= -1.0f;

	if (body.Trigger)
		body.Trigger();

	if (other.Trigger)
		other.Trigger();

	return true;
}

bool Collision::SATCollision(const CircleCollider& body, const Collider& other, FVector2& MTV) {
	float minOverlap = INF;

	FVector2 center = body.GetPosition();
	float		 radius = body.GetRadius();

	std::vector<FVector2> vert = other.m_vertices;

	uint32_t vertCount = other.m_verticesCount;
	uint32_t all = vertCount + 1;

	std::vector<FVector2> axis = std::vector<FVector2>(all);

	for (uint32_t i = 0; i < vertCount; i++)
		axis[i] = PerpendicularAxis(vert, i, vertCount);

	axis[vertCount] = CircleAxis(vert, vertCount, center);

	for (uint32_t i = 0; i < all; i++) {
		auto& a = axis[i];

		FVector2 circleProjection = ProjectCircle(center, radius, a);
		FVector2 otherProjection = ProjectOnto(vert, vertCount, a);

		float overlap = Overlap(circleProjection, otherProjection);

		if (!overlap) {
			MTV = FVector2(0.0f, 0.0f);

			return false;
		}
		else {
			if (overlap < minOverlap) {
				minOverlap = overlap;

				MTV = a * overlap;
			}
		}
	}

	if ((center - GetCenter(other)).Dot(MTV) < 0.0f)
		MTV *= -1.0f;

	if (body.Trigger)
		body.Trigger();

	if (other.Trigger)
		other.Trigger();

	return true;
}

bool Collision::SATCollision(const Collider& body, const CircleCollider& other, FVector2& MTV) {
	bool temp = SATCollision(other, body, MTV);
	MTV *= -1.0f;

	return temp;
}

bool Collision::SATCollision(const CircleCollider& body, const CircleCollider& other, FVector2& MTV) {
	FVector2 diff(body.GetPosition() - other.GetPosition());

	float length = diff.Length();
	float sum = body.GetRadius() + other.GetRadius();

	if (length >= sum)
		return false;

	sum -= length;

	diff = diff.Normalized();

	MTV = diff * sum;

	if (body.Trigger)
		body.Trigger();

	if (other.Trigger)
		other.Trigger();

	return true;
}

FVector2 Collision::GetCenter(const Collider& body) const {
	FVector2 center;

	uint32_t count = body.m_verticesCount;

	float A = 0.0f;

	float area = 0.0f;

	for (uint32_t i = 0; i < count; i++) {
		auto& v0 = body.m_vertices[i];
		auto& v1 = body.m_vertices[i + 1 != count ? i + 1 : 0];

		float b = v0.Dot(v1);

		A += b;

		center += (v0 + v1) * b;
	}

	center *= 1.0f / (3.0f * A);

	return center;
}

FVector2 Collision::CircleAxis(std::vector<FVector2>& vertices, uint32_t count, FVector2 center) {
	FVector2 axis;

	uint32_t index = 0;
	float	 dist = INF;

	for (uint32_t i = 0; i < count; i++) {
		auto& v = vertices[i];

		float d = FVector2::Distance(v, center);

		if (d >= dist)
			continue;

		dist = d;
		index = i;
	}

	return (center - vertices[index]).Normalized();
}

FVector2 Collision::PerpendicularAxis(std::vector<FVector2>& vertices, uint32_t index, uint32_t count) const {
	uint32_t i0 = index + 1;
	uint32_t i1 = index;

	if (i0 == count) {
		i0 = 0;
		i1 = count - 1;
	}

	FVector2 v = (vertices[i0] - vertices[i1]).Normalized();

	return
	{
		-v.y,
		 v.x
	};
}

FVector2 Collision::ProjectOnto(std::vector<FVector2>& vertices, uint32_t count, FVector2 axis) const {
	float min = INF;
	float max = -INF;

	for (uint32_t i = 0; i < count; i++) {
		float projection = vertices[i].Dot(axis);

		if (projection < min)
			min = projection;

		if (projection > max)
			max = projection;
	}

	return
	{
		min,
		max
	};
}

FVector2 Collision::ProjectCircle(FVector2 center, float radius, FVector2 axis) const {
	FVector2 dir = axis * radius;

	FVector2 p1 = center + dir;
	FVector2 p2 = center - dir;

	float min = p1.Dot(axis);
	float max = p2.Dot(axis);

	if (min > max)
		std::swap(min, max);

	return
	{
		min,
		max
	};
}

float Collision::Overlap(FVector2 v0, FVector2 v1) const {
	return !(v0.x <= v1.y && v0.y >= v1.x) ? 0.0f : std::min(v0.y, v1.y) - std::max(v0.x, v1.x);
}

Collision::Collision() {

}

Collision::~Collision() {

}

#pragma warning(default : 6386)
