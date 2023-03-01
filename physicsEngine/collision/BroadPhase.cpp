#include "BroadPhase.h"

BroadPhase::BroadPhase()
{
	m_proxyCount = 0;

	m_pairCapacity = 16;
	m_pairCount = 0;
	m_pairBuffer = std::vector<Pair>(m_pairCapacity);

	m_moveCapacity = 16;
	m_moveCount = 0;
	m_moveBuffer = std::vector<int>(m_moveCapacity);
}

BroadPhase::~BroadPhase()
{
	m_pairBuffer.clear();
	m_moveBuffer.clear();
}

int BroadPhase::CreateProxy(const AABB& aabb, void* userData)
{
	const int proxyId = m_tree.CreateProxy(aabb, userData);
	++m_proxyCount;
	BufferMove(proxyId);
	return proxyId;
}

void BroadPhase::DestroyProxy(int proxyId)
{
	UnBufferMove(proxyId);
	--m_proxyCount;
	m_tree.DestroyProxy(proxyId);
}

void BroadPhase::MoveProxy(int proxyId, const AABB& aabb, const Vec2& displacement)
{
	const bool buffer = m_tree.MoveProxy(proxyId, aabb, displacement);
	if (buffer)
	{
		BufferMove(proxyId);
	}
}

void BroadPhase::TouchProxy(int proxyId)
{
	BufferMove(proxyId);
}

void BroadPhase::BufferMove(int proxyId)
{
	if (m_moveCount == m_moveCapacity)
	{
		m_moveCapacity *= 2;
		m_moveBuffer.resize(m_moveCapacity);
	}

	m_moveBuffer[m_moveCount] = proxyId;
	++m_moveCount;
}

void BroadPhase::UnBufferMove(int proxyId)
{
	for (int i = 0; i < m_moveCount; ++i)
	{
		if (m_moveBuffer[i] == proxyId)
		{
			m_moveBuffer[i] = e_nullProxy;
		}
	}
}

// This is called from DynamicTree::Query when we are gathering pairs.
bool BroadPhase::QueryCallback(int proxyId)
{
	// A proxy cannot form a pair with itself.
	if (proxyId == m_queryProxyId)
	{
		return true;
	}

	const bool moved = m_tree.WasMoved(proxyId);
	if (moved && proxyId > m_queryProxyId)
	{
		// Both proxies are moving. Avoid duplicate pairs.
		return true;
	}

	// Grow the pair buffer as needed.
	if (m_pairCount == m_pairCapacity)
	{
		m_pairCapacity = m_pairCapacity + (m_pairCapacity >> 1);
		m_pairBuffer.resize(m_pairCapacity);
	}

	m_pairBuffer[m_pairCount].proxyIdA = Min(proxyId, m_queryProxyId);
	m_pairBuffer[m_pairCount].proxyIdB = Max(proxyId, m_queryProxyId);
	++m_pairCount;

	return true;
}
