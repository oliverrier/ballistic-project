#pragma once
#include "Shape.h"

/// A solid circle shape
class CircleShape : public Shape
{
public:
	CircleShape();

	/// Implement Shape.
	Shape* Clone() const override;

	/// @see Shape::GetChildCount
	int GetChildCount() const override;

	/// Implement Shape.
	bool TestPoint(const Transform& transform, const Vec2& p) const override;

	/// Implement Shape.
	/// @note because the circle is solid, rays that start inside do not hit because the normal is
	/// not defined.
	bool RayCast(RayCastOutput* output, const RayCastInput& input,
				const Transform& transform, int childIndex) const override;

	/// @see Shape::ComputeAABB
	void ComputeAABB(AABB* aabb, const Transform& transform, int childIndex) const override;

	/// @see Shape::ComputeMass
	void ComputeMass(MassData* massData, float density) const override;

	/// Position
	Vec2 m_p;
};

inline CircleShape::CircleShape()
{
	m_type = e_circle;
	m_radius = 0.0f;
	m_p.SetZero();
}

