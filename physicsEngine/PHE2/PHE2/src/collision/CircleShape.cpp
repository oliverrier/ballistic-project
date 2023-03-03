#include "CircleShape.h"

Shape* CircleShape::Clone() const
{
	CircleShape* clone = new CircleShape;
	*clone = *this;
	return clone;
}

int CircleShape::GetChildCount() const
{
	return 1;
}

bool CircleShape::TestPoint(const Transform& transform, const Vec2& p) const
{
	Vec2 center = transform.p + Mul(transform.q, m_p);
	Vec2 d = p - center;
	return Dot(d, d) <= m_radius * m_radius;
}

// Collision Detection in Interactive 3D Environments by Gino van den Bergen
// From Section 3.1.2
// x = s + a * r
// norm(x) = radius
bool CircleShape::RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
							const Transform& transform, int childIndex) const
{
	B2_NOT_USED(childIndex);

	Vec2 position = transform.p + Mul(transform.q, m_p);
	Vec2 s = input.p1 - position;
	float b = Dot(s, s) - m_radius * m_radius;

	// Solve quadratic equation.
	Vec2 r = input.p2 - input.p1;
	float c =  Dot(s, r);
	float rr = Dot(r, r);
	float sigma = c * c - rr * b;

	// Check for negative discriminant and short segment.
	if (sigma < 0.0f || rr < b2_epsilon)
	{
		return false;
	}

	// Find the point of intersection of the line with the circle.
	float a = -(c + sqrtf(sigma));

	// Is the intersection point on the segment?
	if (0.0f <= a && a <= input.maxFraction * rr)
	{
		a /= rr;
		output->fraction = a;
		output->normal = s + a * r;
		output->normal.Normalize();
		return true;
	}

	return false;
}

void CircleShape::ComputeAABB(AABB* aabb, const Transform& transform, int childIndex) const
{
	B2_NOT_USED(childIndex);

	Vec2 p = transform.p + Mul(transform.q, m_p);
	aabb->lowerBound.Set(p.x - m_radius, p.y - m_radius);
	aabb->upperBound.Set(p.x + m_radius, p.y + m_radius);
}

void CircleShape::ComputeMass(MassData* massData, float density) const
{
	massData->mass = density * b2_pi * m_radius * m_radius;
	massData->center = m_p;

	// inertia about the local origin
	massData->I = massData->mass * (0.5f * m_radius * m_radius + Dot(m_p, m_p));
}
