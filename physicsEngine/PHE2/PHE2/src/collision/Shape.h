#pragma once

#include "Collision.h"

struct Vec2;
struct Transform;

/// This holds the mass data computed for a shape.
struct MassData
{
	/// The mass of the shape, usually in kilograms.
	float mass;

	/// The position of the shape's centroid relative to the shape's origin.
	Vec2 center;

	/// The rotational inertia of the shape about the local origin.
	float I;
};

/// A shape is used for collision detection. You can create a shape however you like.
/// Shapes used for simulation in World are created automatically when a Fixture
/// is created. Shapes may encapsulate a one or more child shapes.
class Shape
{
public:

	enum Type
	{
		e_circle = 0,
		e_edge = 1,
		e_polygon = 2,
		e_chain = 3,
		e_typeCount = 4
	};

	virtual ~Shape() {}

	/// Clone the concrete shape using the provided allocator.
	virtual Shape* Clone() const = 0;

	/// Get the type of this shape. You can use this to down cast to the concrete shape.
	/// @return the shape type.
	Type GetType() const;

	/// Get the number of child primitives.
	virtual int GetChildCount() const = 0;

	/// Test a point for containment in this shape. This only works for convex shapes.
	/// @param xf the shape world transform.
	/// @param p a point in world coordinates.
	virtual bool TestPoint(const Transform& xf, const Vec2& p) const = 0;

	/// Cast a ray against a child shape.
	/// @param output the ray-cast results.
	/// @param input the ray-cast input parameters.
	/// @param transform the transform to be applied to the shape.
	/// @param childIndex the child shape index
	virtual bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
						const Transform& transform, int childIndex) const = 0;

	/// Given a transform, compute the associated axis aligned bounding box for a child shape.
	/// @param aabb returns the axis aligned box.
	/// @param xf the world transform of the shape.
	/// @param childIndex the child shape
	virtual void ComputeAABB(AABB* aabb, const Transform& xf, int childIndex) const = 0;

	/// Compute the mass properties of this shape using its dimensions and density.
	/// The inertia tensor is computed about the local origin.
	/// @param massData returns the mass data for this shape.
	/// @param density the density in kilograms per meter squared.
	virtual void ComputeMass(MassData* massData, float density) const = 0;

	Type m_type;

	/// Radius of a shape. For polygonal shapes this must be b2_polygonRadius. There is no support for
	/// making rounded polygons.
	float m_radius;
};

inline Shape::Type Shape::GetType() const
{
	return m_type;
}

