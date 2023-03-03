#pragma once
#include "Shape.h"

struct b2Hull;

/// A solid convex polygon. It is assumed that the interior of the polygon is to
/// the left of each edge.
/// Polygons have a maximum number of vertices equal to b2_maxPolygonVertices.
/// In most cases you should not need many vertices for a convex polygon.
class PolygonShape : public Shape
{
public:
	PolygonShape();

	/// Implement Shape.
	Shape* Clone() const override;

	/// @see Shape::GetChildCount
	int GetChildCount() const override;

	/// Create a convex hull from the given array of local points.
	/// The count must be in the range [3, b2_maxPolygonVertices].
	/// @warning the points may be re-ordered, even if they form a convex polygon
	/// @warning if this fails then the polygon is invalid
	/// @returns true if valid
	bool Set(const Vec2* points, int count);

	/// Create a polygon from a given convex hull (see b2ComputeHull).
	/// @warning the hull must be valid or this will crash or have unexpected behavior
	void Set(const b2Hull& hull);

	/// Build vertices to represent an axis-aligned box centered on the local origin.
	/// @param hx the half-width.
	/// @param hy the half-height.
	void SetAsBox(float hx, float hy);

	/// Build vertices to represent an oriented box.
	/// @param hx the half-width.
	/// @param hy the half-height.
	/// @param center the center of the box in local coordinates.
	/// @param angle the rotation of the box in local coordinates.
	void SetAsBox(float hx, float hy, const Vec2& center, float angle);

	/// @see Shape::TestPoint
	bool TestPoint(const Transform& transform, const Vec2& p) const override;

	/// Implement Shape.
	/// @note because the polygon is solid, rays that start inside do not hit because the normal is
	/// not defined.
	bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
					const Transform& transform, int childIndex) const override;

	/// @see Shape::ComputeAABB
	void ComputeAABB(AABB* aabb, const Transform& transform, int childIndex) const override;

	/// @see Shape::ComputeMass
	void ComputeMass(MassData* massData, float density) const override;

	/// Validate convexity. This is a very time consuming operation.
	/// @returns true if valid
	bool Validate() const;

	Vec2 m_centroid;
	Vec2 m_vertices[b2_maxPolygonVertices];
	Vec2 m_normals[b2_maxPolygonVertices];
	int m_count;
};

