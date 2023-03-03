#include "EdgeShape.h"
#include "../common/Math.h"

void EdgeShape::SetOneSided(const Vec2& v0, const Vec2& v1, const Vec2& v2, const Vec2& v3)
{
	m_vertex0 = v0;
	m_vertex1 = v1;
	m_vertex2 = v2;
	m_vertex3 = v3;
	m_oneSided = true;
}

void EdgeShape::SetTwoSided(const Vec2& v1, const Vec2& v2)
{
	m_vertex1 = v1;
	m_vertex2 = v2;
	m_oneSided = false;
}

Shape* EdgeShape::Clone() const
{
	EdgeShape* clone = new EdgeShape;
	*clone = *this;
	return clone;
}

int EdgeShape::GetChildCount() const
{
	return 1;
}

bool EdgeShape::TestPoint(const Transform& xf, const Vec2& p) const
{
	B2_NOT_USED(xf);
	B2_NOT_USED(p);
	return false;
}

// p = p1 + t * d
// v = v1 + s * e
// p1 + t * d = v1 + s * e
// s * e - t * d = p1 - v1
bool EdgeShape::RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
							const Transform& xf, int childIndex) const
{
	B2_NOT_USED(childIndex);

	// Put the ray into the edge's frame of reference.
	Vec2 p1 = MulT(xf.q, input.p1 - xf.p);
	Vec2 p2 = MulT(xf.q, input.p2 - xf.p);
	Vec2 d = p2 - p1;

	Vec2 v1 = m_vertex1;
	Vec2 v2 = m_vertex2;
	Vec2 e = v2 - v1;

	// Normal points to the right, looking from v1 at v2
	Vec2 normal(e.y, -e.x);
	normal.Normalize();

	// q = p1 + t * d
	// dot(normal, q - v1) = 0
	// dot(normal, p1 - v1) + t * dot(normal, d) = 0
	float numerator = Dot(normal, v1 - p1);
	if (m_oneSided && numerator > 0.0f)
	{
		return false;
	}

	float denominator = Dot(normal, d);

	if (denominator == 0.0f)
	{
		return false;
	}

	float t = numerator / denominator;
	if (t < 0.0f || input.maxFraction < t)
	{
		return false;
	}

	Vec2 q = p1 + t * d;

	// q = v1 + s * r
	// s = dot(q - v1, r) / dot(r, r)
	Vec2 r = v2 - v1;
	float rr = Dot(r, r);
	if (rr == 0.0f)
	{
		return false;
	}

	float s = Dot(q - v1, r) / rr;
	if (s < 0.0f || 1.0f < s)
	{
		return false;
	}

	output->fraction = t;
	if (numerator > 0.0f)
	{
		output->normal = -Mul(xf.q, normal);
	}
	else
	{
		output->normal = Mul(xf.q, normal);
	}
	return true;
}

void EdgeShape::ComputeAABB(AABB* aabb, const Transform& xf, int childIndex) const
{
	B2_NOT_USED(childIndex);

	Vec2 v1 = Mul(xf, m_vertex1);
	Vec2 v2 = Mul(xf, m_vertex2);

	Vec2 lower = Min(v1, v2);
	Vec2 upper = Max(v1, v2);

	Vec2 r(m_radius, m_radius);
	aabb->lowerBound = lower - r;
	aabb->upperBound = upper + r;
}

void EdgeShape::ComputeMass(MassData* massData, float density) const
{
	B2_NOT_USED(density);

	massData->mass = 0.0f;
	massData->center = 0.5f * (m_vertex1 + m_vertex2);
	massData->I = 0.0f;
}
