#include "Collision.h"

#include <vector>

#include "Distance.h"

void WorldManifold::Initialize(const Manifold* manifold,
                               const Transform& xfA, float radiusA,
                               const Transform& xfB, float radiusB)
{
	if (manifold->pointCount == 0)
	{
		return;
	}

	switch (manifold->type)
	{
	case Manifold::e_circles:
		{
			normal.Set(1.0f, 0.0f);
			Vec2 pointA = Mul(xfA, manifold->localPoint);
			Vec2 pointB = Mul(xfB, manifold->points[0].localPoint);
			if (DistanceSquared(pointA, pointB) > FLT_EPSILON * FLT_EPSILON)
			{
				normal = pointB - pointA;
				normal.Normalize();
			}

			Vec2 cA = pointA + radiusA * normal;
			Vec2 cB = pointB - radiusB * normal;
			points[0] = 0.5f * (cA + cB);
			separations[0] = Dot(cB - cA, normal);
		}
		break;

	case Manifold::e_faceA:
		{
			normal = Mul(xfA.q, manifold->localNormal);
			Vec2 planePoint = Mul(xfA, manifold->localPoint);
			
			for (int i = 0; i < manifold->pointCount; ++i)
			{
				Vec2 clipPoint = Mul(xfB, manifold->points[i].localPoint);
				Vec2 cA = clipPoint + (radiusA - Dot(clipPoint - planePoint, normal)) * normal;
				Vec2 cB = clipPoint - radiusB * normal;
				points[i] = 0.5f * (cA + cB);
				separations[i] = Dot(cB - cA, normal);
			}
		}
		break;

	case Manifold::e_faceB:
		{
			normal = Mul(xfB.q, manifold->localNormal);
			Vec2 planePoint = Mul(xfB, manifold->localPoint);

			for (int i = 0; i < manifold->pointCount; ++i)
			{
				Vec2 clipPoint = Mul(xfA, manifold->points[i].localPoint);
				Vec2 cB = clipPoint + (radiusB - Dot(clipPoint - planePoint, normal)) * normal;
				Vec2 cA = clipPoint - radiusA * normal;
				points[i] = 0.5f * (cA + cB);
				separations[i] = Dot(cA - cB, normal);
			}

			// Ensure normal points from A to B.
			normal = -normal;
		}
		break;
	}
}

void GetPointStates(PointState state1[maxManifoldPoints], PointState state2[maxManifoldPoints],
					  const Manifold* manifold1, const Manifold* manifold2)
{
	for (int i = 0; i < maxManifoldPoints; ++i)
	{
		state1[i] = nullState;
		state2[i] = nullState;
	}

	// Detect persists and removes.
	for (int i = 0; i < manifold1->pointCount; ++i)
	{
		ContactID id = manifold1->points[i].id;

		state1[i] = removeState;

		for (int j = 0; j < manifold2->pointCount; ++j)
		{
			if (manifold2->points[j].id.key == id.key)
			{
				state1[i] = persistState;
				break;
			}
		}
	}

	// Detect persists and adds.
	for (int i = 0; i < manifold2->pointCount; ++i)
	{
		ContactID id = manifold2->points[i].id;

		state2[i] = addState;

		for (int j = 0; j < manifold1->pointCount; ++j)
		{
			if (manifold1->points[j].id.key == id.key)
			{
				state2[i] = persistState;
				break;
			}
		}
	}
}

// From Real-time Collision Detection, p179.
bool AABB::RayCast(RayCastOutput* output, const RayCastInput& input) const
{
	float tmin = -FLT_MAX;
	float tmax = FLT_MAX;

	Vec2 p = input.p1;
	Vec2 d = input.p2 - input.p1;
	Vec2 absD = Abs(d);

	Vec2 normal;

	for (int i = 0; i < 2; ++i)
	{
		if (absD(i) < FLT_EPSILON)
		{
			// Parallel.
			if (p(i) < lowerBound(i) || upperBound(i) < p(i))
			{
				return false;
			}
		}
		else
		{
			float inv_d = 1.0f / d(i);
			float t1 = (lowerBound(i) - p(i)) * inv_d;
			float t2 = (upperBound(i) - p(i)) * inv_d;

			// Sign of the normal vector.
			float s = -1.0f;

			if (t1 > t2)
			{
				Swap(t1, t2);
				s = 1.0f;
			}

			// Push the min up
			if (t1 > tmin)
			{
				normal.SetZero();
				normal(i) = s;
				tmin = t1;
			}

			// Pull the max down
			tmax = Min(tmax, t2);

			if (tmin > tmax)
			{
				return false;
			}
		}
	}

	// Does the ray start inside the box?
	// Does the ray intersect beyond the max fraction?
	if (tmin < 0.0f || input.maxFraction < tmin)
	{
		return false;
	}

	// Intersection.
	output->fraction = tmin;
	output->normal = normal;
	return true;
}

// Sutherland-Hodgman clipping.
int ClipSegmentToLine(ClipVertex vOut[2], const ClipVertex vIn[2],
						const Vec2& normal, float offset, int vertexIndexA)
{
	// Start with no output points
	int count = 0;

	// Calculate the distance of end points to the line
	float distance0 = Dot(normal, vIn[0].v) - offset;
	float distance1 = Dot(normal, vIn[1].v) - offset;

	// If the points are behind the plane
	if (distance0 <= 0.0f) vOut[count++] = vIn[0];
	if (distance1 <= 0.0f) vOut[count++] = vIn[1];

	// If the points are on different sides of the plane
	if (distance0 * distance1 < 0.0f)
	{
		// Find intersection point of edge and plane
		float interp = distance0 / (distance0 - distance1);
		vOut[count].v = vIn[0].v + interp * (vIn[1].v - vIn[0].v);

		// VertexA is hitting edgeB.
		vOut[count].id.cf.indexA = static_cast<unsigned char>(vertexIndexA);
		vOut[count].id.cf.indexB = vIn[0].id.cf.indexB;
		vOut[count].id.cf.typeA = ContactFeature::e_vertex;
		vOut[count].id.cf.typeB = ContactFeature::e_face;
		++count;

		assert(count == 2);
	}

	return count;
}

bool TestOverlap(	const Shape* shapeA, int indexA,
					const Shape* shapeB, int indexB,
					const Transform& xfA, const Transform& xfB)
{
	DistanceInput input;
	input.proxyA.Set(shapeA, indexA);
	input.proxyB.Set(shapeB, indexB);
	input.transformA = xfA;
	input.transformB = xfB;
	input.useRadii = true;

	SimplexCache cache;
	cache.count = 0;

	DistanceOutput output;

	Distance(&output, &cache, &input);

	return output.distance < 10.0f * FLT_EPSILON;
}

// quickhull recursion
static Hull RecurseHull(Vec2 p1, Vec2 p2, Vec2* ps, int count)
{
	Hull hull;
	hull.count = 0;

	if (count == 0)
	{
		return hull;
	}

	// create an edge vector pointing from p1 to p2
	Vec2 e = p2 - p1;
	e.Normalize();

	// discard points left of e and find point furthest to the right of e
	Vec2 rightPoints[maxPolygonVertices]{};
	int rightCount = 0;

	int bestIndex = 0;
	float bestDistance = Cross(ps[bestIndex] - p1, e);
	if (bestDistance > 0.0f)
	{
		rightPoints[rightCount++] = ps[bestIndex];
	}

	for (int i = 1; i < count; ++i)
	{
		float distance = Cross(ps[i] - p1, e);
		if (distance > bestDistance)
		{
			bestIndex = i;
			bestDistance = distance;
		}

		if (distance > 0.0f)
		{
			rightPoints[rightCount++] = ps[i];
		}
	}

	if (bestDistance < 2.0f * linearSlop)
	{
		return hull;
	}

	Vec2 bestPoint = ps[bestIndex];

	// compute hull to the right of p1-bestPoint
	Hull hull1 = RecurseHull(p1, bestPoint, rightPoints, rightCount);

	// compute hull to the right of bestPoint-p2
	Hull hull2 = RecurseHull(bestPoint, p2, rightPoints, rightCount);

	// stich together hulls
	for (int i = 0; i < hull1.count; ++i)
	{
		hull.points[hull.count++] = hull1.points[i];
	}

	hull.points[hull.count++] = bestPoint;

	for (int i = 0; i < hull2.count; ++i)
	{
		hull.points[hull.count++] = hull2.points[i];
	}

	assert(hull.count < maxPolygonVertices);

	return hull;
}

// quickhull algorithm
// - merges vertices based on linearSlop
// - removes collinear points using linearSlop
// - returns an empty hull if it fails
Hull ComputeHull(const std::vector<Vec2>& points, int count)
{
	Hull hull;
	hull.count = 0;

	if (count < 3 || count > maxPolygonVertices)
	{
		// check your data
		return hull;
	}

	count = Min(count, maxPolygonVertices);

	AABB aabb = { {FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX} };

	// Perform aggressive point welding. First point always remains.
	// Also compute the bounding box for later.
	Vec2 ps[maxPolygonVertices];
	int n = 0;
	const float tolSqr = 16.0f * linearSlop * linearSlop;
	for (int i = 0; i < count; ++i)
	{
		aabb.lowerBound = Min(aabb.lowerBound, points[i]);
		aabb.upperBound = Max(aabb.upperBound, points[i]);

		Vec2 vi = points[i];

		bool unique = true;
		for (int j = 0; j < i; ++j)
		{
			Vec2 vj = points[j];

			float distSqr = DistanceSquared(vi, vj);
			if (distSqr < tolSqr)
			{
				unique = false;
				break;
			}
		}

		if (unique)
		{
			ps[n++] = vi;
		}
	}

	if (n < 3)
	{
		// all points very close together, check your data and check your scale
		return hull;
	}

	// Find an extreme point as the first point on the hull
	Vec2 c = aabb.GetCenter();
	int i1 = 0;
	float dsq1 = DistanceSquared(c, ps[i1]);
	for (int i = 1; i < n; ++i)
	{
		float dsq = DistanceSquared(c, ps[i]);
		if (dsq > dsq1)
		{
			i1 = i;
			dsq1 = dsq;
		}
	}

	// remove p1 from working set
	Vec2 p1 = ps[i1];
	ps[i1] = ps[n - 1];
	n = n - 1;

	int i2 = 0;
	float dsq2 = DistanceSquared(p1, ps[i2]);
	for (int i = 1; i < n; ++i)
	{
		float dsq = DistanceSquared(p1, ps[i]);
		if (dsq > dsq2)
		{
			i2 = i;
			dsq2 = dsq;
		}
	}

	// remove p2 from working set
	Vec2 p2 = ps[i2];
	ps[i2] = ps[n - 1];
	n = n - 1;

	// split the points into points that are left and right of the line p1-p2.
	Vec2 rightPoints[maxPolygonVertices - 2];
	int rightCount = 0;

	Vec2 leftPoints[maxPolygonVertices - 2];
	int leftCount = 0;

	Vec2 e = p2 - p1;
	e.Normalize();

	for (int i = 0; i < n; ++i)
	{
		float d = Cross(ps[i] - p1, e);

		// slop used here to skip points that are very close to the line p1-p2
		if (d >= 2.0f * linearSlop)
		{
			rightPoints[rightCount++] = ps[i];
		}
		else if (d <= -2.0f * linearSlop)
		{
			leftPoints[leftCount++] = ps[i];
		}
	}

	// compute hulls on right and left
	Hull hull1 = RecurseHull(p1, p2, rightPoints, rightCount);
	Hull hull2 = RecurseHull(p2, p1, leftPoints, leftCount);

	if (hull1.count == 0 && hull2.count == 0)
	{
		// all points collinear
		return hull;
	}

	// stitch hulls together, preserving CCW winding order
	hull.points[hull.count++] = p1;

	for (int i = 0; i < hull1.count; ++i)
	{
		hull.points[hull.count++] = hull1.points[i];
	}

	hull.points[hull.count++] = p2;

	for (int i = 0; i < hull2.count; ++i)
	{
		hull.points[hull.count++] = hull2.points[i];
	}

	assert(hull.count <= maxPolygonVertices);

	// merge collinear
	bool searching = true;
	while (searching && hull.count > 2)
	{
		searching = false;

		for (int i = 0; i < hull.count; ++i)
		{
			int i1 = i;
			int i2 = (i + 1) % hull.count;
			int i3 = (i + 2) % hull.count;

			Vec2 p1 = hull.points[i1];
			Vec2 p2 = hull.points[i2];
			Vec2 p3 = hull.points[i3];

			Vec2 e = p3 - p1;
			e.Normalize();

			Vec2 v = p2 - p1;
			float distance = Cross(p2 - p1, e);
			if (distance <= 2.0f * linearSlop)
			{
				// remove midpoint from hull
				for (int j = i2; j < hull.count - 1; ++j)
				{
					hull.points[j] = hull.points[j + 1];
				}
				hull.count -= 1;

				// continue searching for collinear points
				searching = true;

				break;
			}
		}
	}

	if (hull.count < 3)
	{
		// all points collinear, shouldn't be reached since this was validated above
		hull.count = 0;
	}

	return hull;
}

bool ValidateHull(const Hull& hull)
{
	if (hull.count < 3 || maxPolygonVertices < hull.count)
	{
		return false;
	}

	// test that every point is behind every edge
	for (int i = 0; i < hull.count; ++i)
	{
		// create an edge vector
		int i1 = i;
		int i2 = i < hull.count - 1 ? i1 + 1 : 0;
		Vec2 p = hull.points[i1];
		Vec2 e = hull.points[i2] - p;
		e.Normalize();

		for (int j = 0; j < hull.count; ++j)
		{
			// skip points that subtend the current edge
			if (j == i1 || j == i2)
			{
				continue;
			}

			float distance = Cross(hull.points[j] - p, e);
			if (distance >= 0.0f)
			{
				return false;
			}
		}
	}

	// test for collinear points
	for (int i = 0; i < hull.count; ++i)
	{
		int i1 = i;
		int i2 = (i + 1) % hull.count;
		int i3 = (i + 2) % hull.count;

		Vec2 p1 = hull.points[i1];
		Vec2 p2 = hull.points[i2];
		Vec2 p3 = hull.points[i3];

		Vec2 e = p3 - p1;
		e.Normalize();

		Vec2 v = p2 - p1;
		float distance = Cross(p2 - p1, e);
		if (distance <= linearSlop)
		{
			// p1-p2-p3 are collinear
			return false;
		}
	}

	return true;
}
