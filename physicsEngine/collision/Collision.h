#pragma once
#include <limits.h>
#include "../common/Math.h"

/// @file
/// Structures and functions used for computing contact points, distance
/// queries, and TOI queries.

class Shape;
class CircleShape;
class EdgeShape;
class PolygonShape;

const unsigned char nullFeature = UCHAR_MAX;

/// The features that intersect to form the contact point
/// This must be 4 bytes or less.
struct ContactFeature
{
	enum Type
	{
		e_vertex = 0,
		e_face = 1
	};

	unsigned char indexA;		///< Feature index on shapeA
	unsigned char indexB;		///< Feature index on shapeB
	unsigned char typeA;		///< The feature type on shapeA
	unsigned char typeB;		///< The feature type on shapeB
};

/// Contact ids to facilitate warm starting.
union ContactID
{
	ContactFeature cf;
	unsigned int key;					///< Used to quickly compare contact ids.
};

/// A manifold point is a contact point belonging to a contact
/// manifold. It holds details related to the geometry and dynamics
/// of the contact points.
/// The local point usage depends on the manifold type:
/// -e_circles: the local center of circleB
/// -e_faceA: the local center of cirlceB or the clip point of polygonB
/// -e_faceB: the clip point of polygonA
/// This structure is stored across time steps, so we keep it small.
/// Note: the impulses are used for internal caching and may not
/// provide reliable contact forces, especially for high speed collisions.
struct ManifoldPoint
{
	Vec2 localPoint;		///< usage depends on manifold type
	float normalImpulse;	///< the non-penetration impulse
	float tangentImpulse;	///< the friction impulse
	ContactID id;			///< uniquely identifies a contact point between two shapes
};

/// A manifold for two touching convex shapes.
/// Box2D supports multiple types of contact:
/// - clip point versus plane with radius
/// - point versus point with radius (circles)
/// The local point usage depends on the manifold type:
/// -e_circles: the local center of circleA
/// -e_faceA: the center of faceA
/// -e_faceB: the center of faceB
/// Similarly the local normal usage:
/// -e_circles: not used
/// -e_faceA: the normal on polygonA
/// -e_faceB: the normal on polygonB
/// We store contacts in this way so that position correction can
/// account for movement, which is critical for continuous physics.
/// All contact scenarios must be expressed in one of these types.
/// This structure is stored across time steps, so we keep it small.
struct Manifold
{
	enum Type
	{
		e_circles,
		e_faceA,
		e_faceB
	};

	ManifoldPoint points[maxManifoldPoints];	///< the points of contact
	Vec2 localNormal;								///< not use for Type::e_points
	Vec2 localPoint;								///< usage depends on manifold type
	Type type;
	int pointCount;								///< the number of manifold points
};

/// This is used to compute the current state of a contact manifold.
struct WorldManifold
{
	/// Evaluate the manifold with supplied transforms. This assumes
	/// modest motion from the original state. This does not change the
	/// point count, impulses, etc. The radii must come from the shapes
	/// that generated the manifold.
	void Initialize(const Manifold* manifold,
					const Transform& xfA, float radiusA,
					const Transform& xfB, float radiusB);

	Vec2 normal;								///< world vector pointing from A to B
	Vec2 points[maxManifoldPoints];		///< world contact point (point of intersection)
	float separations[maxManifoldPoints];	///< a negative value indicates overlap, in meters
};

/// This is used for determining the state of contact points.
enum PointState
{
	nullState,		///< point does not exist
	addState,		///< point was added in the update
	persistState,	///< point persisted across the update
	removeState		///< point was removed in the update
};

/// Compute the point states given two manifolds. The states pertain to the transition from manifold1
/// to manifold2. So state1 is either persist or remove while state2 is either add or persist.
void GetPointStates(PointState state1[maxManifoldPoints], PointState state2[maxManifoldPoints],
					  const Manifold* manifold1, const Manifold* manifold2);

/// Used for computing contact manifolds.
struct ClipVertex
{
	Vec2 v;
	ContactID id;
};

/// Ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
struct RayCastInput
{
	Vec2 p1, p2;
	float maxFraction;
};

/// Ray-cast output data. The ray hits at p1 + fraction * (p2 - p1), where p1 and p2
/// come from RayCastInput.
struct RayCastOutput
{
	Vec2 normal;
	float fraction;
};

/// An axis aligned bounding box.
struct AABB
{
	/// Verify that the bounds are sorted.
	bool IsValid() const;

	/// Get the center of the AABB.
	Vec2 GetCenter() const
	{
		return 0.5f * (lowerBound + upperBound);
	}

	/// Get the extents of the AABB (half-widths).
	Vec2 GetExtents() const
	{
		return 0.5f * (upperBound - lowerBound);
	}

	/// Get the perimeter length
	float GetPerimeter() const
	{
		float wx = upperBound.x - lowerBound.x;
		float wy = upperBound.y - lowerBound.y;
		return 2.0f * (wx + wy);
	}

	/// Combine an AABB into this one.
	void Combine(const AABB& aabb)
	{
		lowerBound = Min(lowerBound, aabb.lowerBound);
		upperBound = Max(upperBound, aabb.upperBound);
	}

	/// Combine two AABBs into this one.
	void Combine(const AABB& aabb1, const AABB& aabb2)
	{
		lowerBound = Min(aabb1.lowerBound, aabb2.lowerBound);
		upperBound = Max(aabb1.upperBound, aabb2.upperBound);
	}

	/// Does this aabb contain the provided AABB.
	bool Contains(const AABB& aabb) const
	{
		bool result = true;
		result = result && lowerBound.x <= aabb.lowerBound.x;
		result = result && lowerBound.y <= aabb.lowerBound.y;
		result = result && aabb.upperBound.x <= upperBound.x;
		result = result && aabb.upperBound.y <= upperBound.y;
		return result;
	}

	bool RayCast(RayCastOutput* output, const RayCastInput& input) const;

	Vec2 lowerBound;	///< the lower vertex
	Vec2 upperBound;	///< the upper vertex
};

/// Compute the collision manifold between two circles.
void CollideCircles(Manifold* manifold,
					  const CircleShape* circleA, const Transform& xfA,
					  const CircleShape* circleB, const Transform& xfB);

/// Compute the collision manifold between a polygon and a circle.
void CollidePolygonAndCircle(Manifold* manifold,
							   const PolygonShape* polygonA, const Transform& xfA,
							   const CircleShape* circleB, const Transform& xfB);

/// Compute the collision manifold between two polygons.
void CollidePolygons(Manifold* manifold,
					   const PolygonShape* polygonA, const Transform& xfA,
					   const PolygonShape* polygonB, const Transform& xfB);

/// Compute the collision manifold between an edge and a circle.
void CollideEdgeAndCircle(Manifold* manifold,
							   const EdgeShape* polygonA, const Transform& xfA,
							   const CircleShape* circleB, const Transform& xfB);

/// Compute the collision manifold between an edge and a polygon.
void CollideEdgeAndPolygon(Manifold* manifold,
							   const EdgeShape* edgeA, const Transform& xfA,
							   const PolygonShape* polygonB, const Transform& xfB);

/// Clipping for contact manifolds.
int ClipSegmentToLine(ClipVertex vOut[2], const ClipVertex vIn[2],
							const Vec2& normal, float offset, int vertexIndexA);

/// Determine if two generic shapes overlap.
bool TestOverlap(	const Shape* shapeA, int indexA,
					const Shape* shapeB, int indexB,
					const Transform& xfA, const Transform& xfB);

/// Convex hull used for polygon collision
struct Hull
{
	Vec2 points[maxPolygonVertices];
	int count;
};

/// Compute the convex hull of a set of points. Returns an empty hull if it fails.
/// Some failure cases:
/// - all points very close together
/// - all points on a line
/// - less than 3 points
/// - more than maxPolygonVertices points
/// This welds close points and removes collinear points.
Hull ComputeHull(const Vec2* points, int count);

/// This determines if a hull is valid. Checks for:
/// - convexity
/// - collinear points
/// This is expensive and should not be called at runtime.
bool ValidateHull(const Hull& hull);


// ---------------- Inline Functions ------------------------------------------

inline bool AABB::IsValid() const
{
	Vec2 d = upperBound - lowerBound;
	bool valid = d.x >= 0.0f && d.y >= 0.0f;
	valid = valid && lowerBound.IsValid() && upperBound.IsValid();
	return valid;
}

inline bool IsOverlapping(const AABB& a, const AABB& b)
{
	Vec2 d1, d2;
	d1 = b.lowerBound - a.upperBound;
	d2 = a.lowerBound - b.upperBound;

	if (d1.x > 0.0f || d1.y > 0.0f)
		return false;

	if (d2.x > 0.0f || d2.y > 0.0f)
		return false;

	return true;
}
