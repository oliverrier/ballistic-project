#pragma once

#include "Collision.h"
#include "DynamicTree.h"

struct Pair
{
	int proxyIdA;
	int proxyIdB;
};

/// The broad-phase is used for computing pairs and performing volume queries and ray casts.
/// This broad-phase does not persist pairs. Instead, this reports potentially new pairs.
/// It is up to the client to consume the new pairs and to track subsequent overlap.
class BroadPhase
{
public:

	enum
	{
		e_nullProxy = -1
	};

	BroadPhase();
	~BroadPhase();

	/// Create a proxy with an initial AABB. Pairs are not reported until
	/// UpdatePairs is called.
	int CreateProxy(const AABB& aabb, void* userData);

	/// Destroy a proxy. It is up to the client to remove any pairs.
	void DestroyProxy(int proxyId);

	/// Call MoveProxy as many times as you like, then when you are done
	/// call UpdatePairs to finalized the proxy pairs (for your time step).
	void MoveProxy(int proxyId, const AABB& aabb, const Vec2& displacement);

	/// Call to trigger a re-processing of it's pairs on the next call to UpdatePairs.
	void TouchProxy(int proxyId);

	/// Get the fat AABB for a proxy.
	const AABB& GetFatAABB(int proxyId) const;

	/// Get user data from a proxy. Returns nullptr if the id is invalid.
	void* GetUserData(int proxyId) const;

	/// Test overlap of fat AABBs.
	bool TestOverlap(int proxyIdA, int proxyIdB) const;

	/// Get the number of proxies.
	int GetProxyCount() const;

	/// Update the pairs. This results in pair callbacks. This can only add pairs.
	template <typename T>
	void UpdatePairs(T* callback);

	/// Query an AABB for overlapping proxies. The callback class
	/// is called for each proxy that overlaps the supplied AABB.
	template <typename T>
	void Query(T* callback, const AABB& aabb) const;

	/// Ray-cast against the proxies in the tree. This relies on the callback
	/// to perform a exact ray-cast in the case were the proxy contains a shape.
	/// The callback also performs the any collision filtering. This has performance
	/// roughly equal to k * log(n), where k is the number of collisions and n is the
	/// number of proxies in the tree.
	/// @param input the ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
	/// @param callback a callback class that is called for each proxy that is hit by the ray.
	template <typename T>
	void RayCast(T* callback, const RayCastInput& input) const;

	/// Get the height of the embedded tree.
	int GetTreeHeight() const;

	/// Get the balance of the embedded tree.
	int GetTreeBalance() const;

	/// Get the quality metric of the embedded tree.
	float GetTreeQuality() const;

	/// Shift the world origin. Useful for large worlds.
	/// The shift formula is: position -= newOrigin
	/// @param newOrigin the new origin with respect to the old origin
	void ShiftOrigin(const Vec2& newOrigin);

private:

	friend class DynamicTree;

	void BufferMove(int proxyId);
	void UnBufferMove(int proxyId);

	bool QueryCallback(int proxyId);

	DynamicTree m_tree;

	int m_proxyCount;

	std::vector<int> m_moveBuffer;
	int m_moveCapacity;
	int m_moveCount;

	std::vector<Pair> m_pairBuffer;
	int m_pairCapacity;
	int m_pairCount;

	int m_queryProxyId;
};

inline void* BroadPhase::GetUserData(int proxyId) const
{
	return m_tree.GetUserData(proxyId);
}

inline bool BroadPhase::TestOverlap(int proxyIdA, int proxyIdB) const
{
	const AABB& aabbA = m_tree.GetFatAABB(proxyIdA);
	const AABB& aabbB = m_tree.GetFatAABB(proxyIdB);
	return IsOverlapping(aabbA, aabbB);
}

inline const AABB& BroadPhase::GetFatAABB(int proxyId) const
{
	return m_tree.GetFatAABB(proxyId);
}

inline int BroadPhase::GetProxyCount() const
{
	return m_proxyCount;
}

inline int BroadPhase::GetTreeHeight() const
{
	return m_tree.GetHeight();
}

inline int BroadPhase::GetTreeBalance() const
{
	return m_tree.GetMaxBalance();
}

inline float BroadPhase::GetTreeQuality() const
{
	return m_tree.GetAreaRatio();
}

template <typename T>
void BroadPhase::UpdatePairs(T* callback)
{
	// Reset pair buffer
	m_pairCount = 0;

	// Perform tree queries for all moving proxies.
	for (int i = 0; i < m_moveCount; ++i)
	{
		m_queryProxyId = m_moveBuffer[i];
		if (m_queryProxyId == e_nullProxy)
		{
			continue;
		}

		// We have to query the tree with the fat AABB so that
		// we don't fail to create a pair that may touch later.
		const AABB& fatAABB = m_tree.GetFatAABB(m_queryProxyId);

		// Query tree, create pairs and add them pair buffer.
		m_tree.Query(this, fatAABB);
	}

	// Send pairs to caller
	for (int i = 0; i < m_pairCount; ++i)
	{
		Pair primaryPair = m_pairBuffer[i];
		void* userDataA = m_tree.GetUserData(primaryPair.proxyIdA);
		void* userDataB = m_tree.GetUserData(primaryPair.proxyIdB);

		callback->AddPair(userDataA, userDataB);
	}

	// Clear move flags
	for (int i = 0; i < m_moveCount; ++i)
	{
		int proxyId = m_moveBuffer[i];
		if (proxyId == e_nullProxy)
		{
			continue;
		}

		m_tree.ClearMoved(proxyId);
	}

	// Reset move buffer
	m_moveCount = 0;
}

template <typename T>
inline void BroadPhase::Query(T* callback, const AABB& aabb) const
{
	m_tree.Query(callback, aabb);
}

template <typename T>
inline void BroadPhase::RayCast(T* callback, const RayCastInput& input) const
{
	m_tree.RayCast(callback, input);
}

inline void BroadPhase::ShiftOrigin(const Vec2& newOrigin)
{
	m_tree.ShiftOrigin(newOrigin);
}
