#pragma once
#include "../common/Math.h"

class Shape;

/// A distance proxy is used by the GJK algorithm.
/// It encapsulates any shape.
struct DistanceProxy
{
	DistanceProxy() : m_vertices(nullptr), m_count(0), m_radius(0.0f) {}

	/// Initialize the proxy using the given shape. The shape
	/// must remain in scope while the proxy is in use.
	void Set(const Shape* shape, int index);

    /// Initialize the proxy using a vertex cloud and radius. The vertices
    /// must remain in scope while the proxy is in use.
    void Set(const Vec2* vertices, int count, float radius);

	/// Get the supporting vertex index in the given direction.
	int GetSupport(const Vec2& d) const;

	/// Get the supporting vertex in the given direction.
	const Vec2& GetSupportVertex(const Vec2& d) const;

	/// Get the vertex count.
	int GetVertexCount() const;

	/// Get a vertex by index. Used by Distance.
	const Vec2& GetVertex(int index) const;

	Vec2 m_buffer[2];
	const Vec2* m_vertices;
	int m_count;
	float m_radius;
};

/// Used to warm start Distance.
/// Set count to zero on first call.
struct SimplexCache
{
	float metric;		///< length or area
	unsigned short count;
	unsigned char indexA[3];	///< vertices on shape A
	unsigned char indexB[3];	///< vertices on shape B
};

/// Input for Distance.
/// You have to option to use the shape radii
/// in the computation. Even
struct DistanceInput
{
	DistanceProxy proxyA;
	DistanceProxy proxyB;
	Transform transformA;
	Transform transformB;
	bool useRadii;
};

/// Output for Distance.
struct DistanceOutput
{
	Vec2 pointA;		///< closest point on shapeA
	Vec2 pointB;		///< closest point on shapeB
	float distance;
	int iterations;	///< number of GJK iterations used
};

/// Compute the closest points between two shapes. Supports any combination of:
/// CircleShape, PolygonShape, EdgeShape. The simplex cache is input/output.
/// On the first call set SimplexCache.count to zero.
void Distance(DistanceOutput* output,
				SimplexCache* cache,
				const DistanceInput* input);

/// Input parameters for ShapeCast
struct ShapeCastInput
{
	DistanceProxy proxyA;
	DistanceProxy proxyB;
	Transform transformA;
	Transform transformB;
	Vec2 translationB;
};

/// Output results for ShapeCast
struct ShapeCastOutput
{
	Vec2 point;
	Vec2 normal;
	float lambda;
	int iterations;
};

/// Perform a linear shape cast of shape B moving and shape A fixed. Determines the hit point, normal, and translation fraction.
/// @returns true if hit, false if there is no hit or an initial overlap
bool ShapeCast(ShapeCastOutput* output, const ShapeCastInput* input);

//////////////////////////////////////////////////////////////////////////

inline int DistanceProxy::GetVertexCount() const
{
	return m_count;
}

inline const Vec2& DistanceProxy::GetVertex(int index) const
{
	b2Assert(0 <= index && index < m_count);
	return m_vertices[index];
}

inline int DistanceProxy::GetSupport(const Vec2& d) const
{
	int bestIndex = 0;
	float bestValue = Dot(m_vertices[0], d);
	for (int i = 1; i < m_count; ++i)
	{
		float value = Dot(m_vertices[i], d);
		if (value > bestValue)
		{
			bestIndex = i;
			bestValue = value;
		}
	}

	return bestIndex;
}

inline const Vec2& DistanceProxy::GetSupportVertex(const Vec2& d) const
{
	int bestIndex = 0;
	float bestValue = Dot(m_vertices[0], d);
	for (int i = 1; i < m_count; ++i)
	{
		float value = Dot(m_vertices[i], d);
		if (value > bestValue)
		{
			bestIndex = i;
			bestValue = value;
		}
	}

	return m_vertices[bestIndex];
}
