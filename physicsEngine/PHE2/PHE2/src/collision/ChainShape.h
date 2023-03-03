#pragma once
#include <vector>

#include "Collision.h"
#include "Shape.h"

class EdgeShape;

/// A chain shape is a free form sequence of line segments.
/// The chain has one-sided collision, with the surface normal pointing to the right of the edge.
/// This provides a counter-clockwise winding like the polygon shape.
/// Connectivity information is used to create smooth collisions.
/// @warning the chain will not collide properly if there are self-intersections.
class ChainShape : public Shape
{
public:
	ChainShape();

	/// The destructor frees the vertices using b2Free.
	~ChainShape();

	/// Clear all data.
	void Clear();

	/// Create a loop. This automatically adjusts connectivity.
	/// @param vertices an array of vertices, these are copied
	/// @param count the vertex count
	void CreateLoop(const Vec2* vertices, int count);

	/// Create a chain with ghost vertices to connect multiple chains together.
	/// @param vertices an array of vertices, these are copied
	/// @param count the vertex count
	/// @param prevVertex previous vertex from chain that connects to the start
	/// @param nextVertex next vertex from chain that connects to the end
	void CreateChain(const std::vector<Vec2*>& vertices, int count);

	/// Implement Shape. Vertices are cloned using b2Alloc.
	Shape* Clone() const override;

	/// @see Shape::GetChildCount
	int GetChildCount() const override;

	/// Get a child edge.
	void GetChildEdge(EdgeShape* edge, int index) const;

	/// This always return false.
	/// @see Shape::TestPoint
	bool TestPoint(const Transform& transform, const Vec2& p) const override;

	/// Implement Shape.
	bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
					const Transform& transform, int childIndex) const override;

	/// @see Shape::ComputeAABB
	void ComputeAABB(AABB* aabb, const Transform& transform, int childIndex) const override;

	/// Chains have zero mass.
	/// @see Shape::ComputeMass
	void ComputeMass(MassData* massData, float density) const override;

	/// The vertices. Owned by this class.
	std::vector<Vec2*> m_vertices;

	/// The vertex count.
	int m_count;
};

inline ChainShape::ChainShape()
{
	m_type = e_chain;
	m_radius = b2_polygonRadius;
	m_vertices = {};
	m_count = 0;
}

