#include "ChainShape.h"
#include "EdgeShape.h"


ChainShape::~ChainShape()
{
	Clear();
}

void ChainShape::Clear()
{
	m_vertices.clear();
	m_vertices = {};
	m_count = 0;
}

void ChainShape::CreateLoop(const std::vector<Vec2>& vertices, int count)
{
	assert(m_vertices.size() == 0 && m_count == 0);
	assert(count >= 3);
	if (count < 3)
	{
		return;
	}

	for (int i = 1; i < count; ++i)
	{
		Vec2 v1 = vertices[i-1];
		Vec2 v2 = vertices[i];
		// If the code crashes here, it means your vertices are too close together.
		assert(DistanceSquared(v1, v2) > linearSlop * linearSlop);
	}

	m_count = count + 1;
	m_vertices = vertices;
	m_vertices.resize(m_count);
	m_vertices[count] = vertices[0];
	m_prevVertex = m_vertices[m_count - 2];
	m_nextVertex = m_vertices[1];
}

void ChainShape::CreateChain(const std::vector<Vec2>& vertices, int count, const Vec2& prevVertex, const Vec2& nextVertex)
{
	assert(m_vertices.size() == 0 && m_count == 0);
	assert(count >= 2);
	for (int i = 1; i < count; ++i)
	{
		// If the code crashes here, it means your vertices are too close together.
		assert(DistanceSquared(vertices[i-1], vertices[i]) > linearSlop * linearSlop);
	}

	m_count = count;
	m_vertices = vertices;
	m_prevVertex = prevVertex;
	m_nextVertex = nextVertex;
}

Shape* ChainShape::Clone() const
{
	ChainShape* clone = new ChainShape;
	clone->CreateChain(m_vertices, m_count, m_prevVertex, m_nextVertex);
	return clone;
}

int ChainShape::GetChildCount() const
{
	// edge count = vertex count - 1
	return m_count - 1;
}

void ChainShape::GetChildEdge(EdgeShape* edge, int index) const
{
	assert(0 <= index && index < m_count - 1);
	edge->m_type = Shape::e_edge;
	edge->m_radius = m_radius;

	edge->m_vertex1 = m_vertices[index + 0];
	edge->m_vertex2 = m_vertices[index + 1];
	edge->m_oneSided = true;

	if (index > 0)
	{
		edge->m_vertex0 = m_vertices[index - 1];
	}
	else
	{
		edge->m_vertex0 = m_prevVertex;
	}

	if (index < m_count - 2)
	{
		edge->m_vertex3 = m_vertices[index + 2];
	}
	else
	{
		edge->m_vertex3 = m_nextVertex;
	}
}

bool ChainShape::TestPoint(const Transform& xf, const Vec2& p) const
{
	NOT_USED(xf);
	NOT_USED(p);
	return false;
}

bool ChainShape::RayCast(RayCastOutput* output, const RayCastInput& input,
							const Transform& xf, int childIndex) const
{
	assert(childIndex < m_count);

	EdgeShape edgeShape;

	int i1 = childIndex;
	int i2 = childIndex + 1;
	if (i2 == m_count)
	{
		i2 = 0;
	}

	edgeShape.m_vertex1 = m_vertices[i1];
	edgeShape.m_vertex2 = m_vertices[i2];

	return edgeShape.RayCast(output, input, xf, 0);
}

void ChainShape::ComputeAABB(AABB* aabb, const Transform& xf, int childIndex) const
{
	assert(childIndex < m_count);

	int i1 = childIndex;
	int i2 = childIndex + 1;
	if (i2 == m_count)
	{
		i2 = 0;
	}

	Vec2 v1 = Mul(xf, m_vertices[i1]);
	Vec2 v2 = Mul(xf, m_vertices[i2]);

	Vec2 lower = Min(v1, v2);
	Vec2 upper = Max(v1, v2);

	Vec2 r(m_radius, m_radius);
	aabb->lowerBound = lower - r;
	aabb->upperBound = upper + r;
}

void ChainShape::ComputeMass(MassData* massData, float density) const
{
	NOT_USED(density);

	massData->mass = 0.0f;
	massData->center.SetZero();
	massData->I = 0.0f;
}
