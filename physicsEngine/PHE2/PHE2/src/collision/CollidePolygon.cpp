#include "PolygonShape.h"
// Find the max separation between poly1 and poly2 using edge normals from poly1.
static float b2FindMaxSeparation(int* edgeIndex,
								 const PolygonShape* poly1, const Transform& xf1,
								 const PolygonShape* poly2, const Transform& xf2)
{
	int count1 = poly1->m_count;
	int count2 = poly2->m_count;
	const Vec2* n1s = poly1->m_normals;
	const Vec2* v1s = poly1->m_vertices;
	const Vec2* v2s = poly2->m_vertices;
	Transform xf = MulT(xf2, xf1);

	int bestIndex = 0;
	float maxSeparation = -b2_maxFloat;
	for (int i = 0; i < count1; ++i)
	{
		// Get poly1 normal in frame2.
		Vec2 n = Mul(xf.q, n1s[i]);
		Vec2 v1 = Mul(xf, v1s[i]);

		// Find deepest point for normal i.
		float si = b2_maxFloat;
		for (int j = 0; j < count2; ++j)
		{
			float sij = Dot(n, v2s[j] - v1);
			if (sij < si)
			{
				si = sij;
			}
		}

		if (si > maxSeparation)
		{
			maxSeparation = si;
			bestIndex = i;
		}
	}

	*edgeIndex = bestIndex;
	return maxSeparation;
}

static void b2FindIncidentEdge(b2ClipVertex c[2],
							 const PolygonShape* poly1, const Transform& xf1, int edge1,
							 const PolygonShape* poly2, const Transform& xf2)
{
	const Vec2* normals1 = poly1->m_normals;

	int count2 = poly2->m_count;
	const Vec2* vertices2 = poly2->m_vertices;
	const Vec2* normals2 = poly2->m_normals;

	b2Assert(0 <= edge1 && edge1 < poly1->m_count);

	// Get the normal of the reference edge in poly2's frame.
	Vec2 normal1 = MulT(xf2.q, Mul(xf1.q, normals1[edge1]));

	// Find the incident edge on poly2.
	int index = 0;
	float minDot = b2_maxFloat;
	for (int i = 0; i < count2; ++i)
	{
		float dot = Dot(normal1, normals2[i]);
		if (dot < minDot)
		{
			minDot = dot;
			index = i;
		}
	}

	// Build the clip vertices for the incident edge.
	int i1 = index;
	int i2 = i1 + 1 < count2 ? i1 + 1 : 0;

	c[0].v = Mul(xf2, vertices2[i1]);
	c[0].id.cf.indexA = (unsigned char)edge1;
	c[0].id.cf.indexB = (unsigned char)i1;
	c[0].id.cf.typeA = b2ContactFeature::e_face;
	c[0].id.cf.typeB = b2ContactFeature::e_vertex;

	c[1].v = Mul(xf2, vertices2[i2]);
	c[1].id.cf.indexA = (unsigned char)edge1;
	c[1].id.cf.indexB = (unsigned char)i2;
	c[1].id.cf.typeA = b2ContactFeature::e_face;
	c[1].id.cf.typeB = b2ContactFeature::e_vertex;
}

// Find edge normal of max separation on A - return if separating axis is found
// Find edge normal of max separation on B - return if separation axis is found
// Choose reference edge as min(minA, minB)
// Find incident edge
// Clip

// The normal points from 1 to 2
void b2CollidePolygons(Manifold* manifold,
					  const PolygonShape* polyA, const Transform& xfA,
					  const PolygonShape* polyB, const Transform& xfB)
{
	manifold->pointCount = 0;
	float totalRadius = polyA->m_radius + polyB->m_radius;

	int edgeA = 0;
	float separationA = b2FindMaxSeparation(&edgeA, polyA, xfA, polyB, xfB);
	if (separationA > totalRadius)
		return;

	int edgeB = 0;
	float separationB = b2FindMaxSeparation(&edgeB, polyB, xfB, polyA, xfA);
	if (separationB > totalRadius)
		return;

	const PolygonShape* poly1;	// reference polygon
	const PolygonShape* poly2;	// incident polygon
	Transform xf1, xf2;
	int edge1;					// reference edge
	unsigned char flip;
	const float k_tol = 0.1f * b2_linearSlop;

	if (separationB > separationA + k_tol)
	{
		poly1 = polyB;
		poly2 = polyA;
		xf1 = xfB;
		xf2 = xfA;
		edge1 = edgeB;
		manifold->type = Manifold::e_faceB;
		flip = 1;
	}
	else
	{
		poly1 = polyA;
		poly2 = polyB;
		xf1 = xfA;
		xf2 = xfB;
		edge1 = edgeA;
		manifold->type = Manifold::e_faceA;
		flip = 0;
	}

	b2ClipVertex incidentEdge[2];
	b2FindIncidentEdge(incidentEdge, poly1, xf1, edge1, poly2, xf2);

	int count1 = poly1->m_count;
	const Vec2* vertices1 = poly1->m_vertices;

	int iv1 = edge1;
	int iv2 = edge1 + 1 < count1 ? edge1 + 1 : 0;

	Vec2 v11 = vertices1[iv1];
	Vec2 v12 = vertices1[iv2];

	Vec2 localTangent = v12 - v11;
	localTangent.Normalize();
	
	Vec2 localNormal = Cross(localTangent, 1.0f);
	Vec2 planePoint = 0.5f * (v11 + v12);

	Vec2 tangent = Mul(xf1.q, localTangent);
	Vec2 normal = Cross(tangent, 1.0f);
	
	v11 = Mul(xf1, v11);
	v12 = Mul(xf1, v12);

	// Face offset.
	float frontOffset = Dot(normal, v11);

	// Side offsets, extended by polytope skin thickness.
	float sideOffset1 = -Dot(tangent, v11) + totalRadius;
	float sideOffset2 = Dot(tangent, v12) + totalRadius;

	// Clip incident edge against extruded edge1 side edges.
	b2ClipVertex clipPoints1[2];
	b2ClipVertex clipPoints2[2];
	int np;

	// Clip to box side 1
	np = b2ClipSegmentToLine(clipPoints1, incidentEdge, -tangent, sideOffset1, iv1);

	if (np < 2)
		return;

	// Clip to negative box side 1
	np = b2ClipSegmentToLine(clipPoints2, clipPoints1,  tangent, sideOffset2, iv2);

	if (np < 2)
	{
		return;
	}

	// Now clipPoints2 contains the clipped points.
	manifold->localNormal = localNormal;
	manifold->localPoint = planePoint;

	int pointCount = 0;
	for (int i = 0; i < b2_maxManifoldPoints; ++i)
	{
		float separation = Dot(normal, clipPoints2[i].v) - frontOffset;

		if (separation <= totalRadius)
		{
			b2ManifoldPoint* cp = manifold->points + pointCount;
			cp->localPoint = MulT(xf2, clipPoints2[i].v);
			cp->id = clipPoints2[i].id;
			if (flip)
			{
				// Swap features
				b2ContactFeature cf = cp->id.cf;
				cp->id.cf.indexA = cf.indexB;
				cp->id.cf.indexB = cf.indexA;
				cp->id.cf.typeA = cf.typeB;
				cp->id.cf.typeB = cf.typeA;
			}
			++pointCount;
		}
	}

	manifold->pointCount = pointCount;
}
