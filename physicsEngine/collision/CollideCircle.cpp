#include "Collision.h"
#include "CircleShape.h"
#include "PolygonShape.h"

void CollideCircles(
	Manifold* manifold,
	const CircleShape* circleA, const Transform& xfA,
	const CircleShape* circleB, const Transform& xfB)
{
	manifold->pointCount = 0;

	Vec2 pA = Mul(xfA, circleA->m_p);
	Vec2 pB = Mul(xfB, circleB->m_p);

	Vec2 d = pB - pA;
	float distSqr = Dot(d, d);
	float rA = circleA->m_radius, rB = circleB->m_radius;
	float radius = rA + rB;
	if (distSqr > radius * radius)
	{
		return;
	}

	manifold->type = Manifold::e_circles;
	manifold->localPoint = circleA->m_p;
	manifold->localNormal.SetZero();
	manifold->pointCount = 1;

	manifold->points[0].localPoint = circleB->m_p;
	manifold->points[0].id.key = 0;
}

void CollidePolygonAndCircle(
	Manifold* manifold,
	const PolygonShape* polygonA, const Transform& xfA,
	const CircleShape* circleB, const Transform& xfB)
{
	manifold->pointCount = 0;

	// Compute circle position in the frame of the polygon.
	Vec2 c = Mul(xfB, circleB->m_p);
	Vec2 cLocal = MulT(xfA, c);

	// Find the min separating edge.
	int normalIndex = 0;
	float separation = -FLT_MAX;
	float radius = polygonA->m_radius + circleB->m_radius;
	int vertexCount = polygonA->m_count;
	const Vec2* vertices = polygonA->m_vertices;
	const Vec2* normals = polygonA->m_normals;

	for (int i = 0; i < vertexCount; ++i)
	{
		float s = Dot(normals[i], cLocal - vertices[i]);

		if (s > radius)
		{
			// Early out.
			return;
		}

		if (s > separation)
		{
			separation = s;
			normalIndex = i;
		}
	}

	// Vertices that subtend the incident face.
	int vertIndex1 = normalIndex;
	int vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
	Vec2 v1 = vertices[vertIndex1];
	Vec2 v2 = vertices[vertIndex2];

	// If the center is inside the polygon ...
	if (separation < FLT_EPSILON)
	{
		manifold->pointCount = 1;
		manifold->type = Manifold::e_faceA;
		manifold->localNormal = normals[normalIndex];
		manifold->localPoint = 0.5f * (v1 + v2);
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
		return;
	}

	// Compute barycentric coordinates
	float u1 = Dot(cLocal - v1, v2 - v1);
	float u2 = Dot(cLocal - v2, v1 - v2);
	if (u1 <= 0.0f)
	{
		if (DistanceSquared(cLocal, v1) > radius * radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::e_faceA;
		manifold->localNormal = cLocal - v1;
		manifold->localNormal.Normalize();
		manifold->localPoint = v1;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
	else if (u2 <= 0.0f)
	{
		if (DistanceSquared(cLocal, v2) > radius * radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::e_faceA;
		manifold->localNormal = cLocal - v2;
		manifold->localNormal.Normalize();
		manifold->localPoint = v2;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
	else
	{
		Vec2 faceCenter = 0.5f * (v1 + v2);
		float s = Dot(cLocal - faceCenter, normals[vertIndex1]);
		if (s > radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::e_faceA;
		manifold->localNormal = normals[vertIndex1];
		manifold->localPoint = faceCenter;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
}
