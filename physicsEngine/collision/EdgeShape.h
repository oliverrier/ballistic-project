#pragma once

#include "Shape.h"

/// A line segment (edge) shape. These can be connected in chains or loops
/// to other edge shapes. Edges created independently are two-sided and do
/// no provide smooth movement across junctions.
class EdgeShape : public Shape
{
public:
	EdgeShape();

	/// Set this as a part of a sequence. Vertex v0 precedes the edge and vertex v3
	/// follows. These extra vertices are used to provide smooth movement
	/// across junctions. This also makes the collision one-sided. The edge
	/// normal points to the right looking from v1 to v2.
	void SetOneSided(const Vec2& v0, const Vec2& v1,const Vec2& v2, const Vec2& v3);

	/// Set this as an isolated edge. Collision is two-sided.
	void SetTwoSided(const Vec2& v1, const Vec2& v2);

	/// Implement Shape.
	Shape* Clone() const override;

	/// @see Shape::GetChildCount
	int GetChildCount() const override;

	/// @see Shape::TestPoint
	bool TestPoint(const Transform& transform, const Vec2& p) const override;

	/// Implement Shape.
	bool RayCast(RayCastOutput* output, const RayCastInput& input,
				const Transform& transform, int childIndex) const override;

	/// @see Shape::ComputeAABB
	void ComputeAABB(AABB* aabb, const Transform& transform, int childIndex) const override;

	/// @see Shape::ComputeMass
	void ComputeMass(MassData* massData, float density) const override;

	/// These are the edge vertices
	Vec2 m_vertex1, m_vertex2;

	/// Optional adjacent vertices. These are used for smooth collision.
	Vec2 m_vertex0, m_vertex3;

	/// Uses m_vertex0 and m_vertex3 to create smooth collision.
	bool m_oneSided;
};

inline EdgeShape::EdgeShape()
{
	m_type = e_edge;
	m_radius = polygonRadius;
	m_vertex0.x = 0.0f;
	m_vertex0.y = 0.0f;
	m_vertex3.x = 0.0f;
	m_vertex3.y = 0.0f;
	m_oneSided = false;
}

