#pragma once
#include <vector>

#include "Collision.h"
#include "../common/Common.h"

#define b2_nullNode (-1)

struct Vec2;

/// A node in the dynamic tree. The client does not interact with this directly.
struct TreeNode
{
	bool IsLeaf() const
	{
		return child1 == b2_nullNode;
	}

	/// Enlarged AABB
	AABB aabb;

	void* userData;

	union
	{
		int parent;
		int next;
	};

	int child1;
	int child2;

	// leaf = 0, free node = -1
	int height;

	bool moved;
};

/// A dynamic AABB tree broad-phase, inspired by Nathanael Presson's btDbvt.
/// A dynamic tree arranges data in a binary tree to accelerate
/// queries such as volume queries and ray casts. Leafs are proxies
/// with an AABB. In the tree we expand the proxy AABB by b2_fatAABBFactor
/// so that the proxy AABB is bigger than the client object. This allows the client
/// object to move by small amounts without triggering a tree update.
///
/// Nodes are pooled and relocatable, so we use node indices rather than pointers.
class DynamicTree
{
public:
	/// Constructing the tree initializes the node pool.
	DynamicTree();

	/// Destroy the tree, freeing the node pool.
	~DynamicTree();

	/// Create a proxy. Provide a tight fitting AABB and a userData pointer.
	int CreateProxy(const AABB& aabb, void* userData);

	/// Destroy a proxy. This asserts if the id is invalid.
	void DestroyProxy(int proxyId);

	/// Move a proxy with a swepted AABB. If the proxy has moved outside of its fattened AABB,
	/// then the proxy is removed from the tree and re-inserted. Otherwise
	/// the function returns immediately.
	/// @return true if the proxy was re-inserted.
	bool MoveProxy(int proxyId, const AABB& aabb1, const Vec2& displacement);

	/// Get proxy user data.
	/// @return the proxy user data or 0 if the id is invalid.
	void* GetUserData(int proxyId) const;

	bool WasMoved(int proxyId) const;
	void ClearMoved(int proxyId);

	/// Get the fat AABB for a proxy.
	const AABB& GetFatAABB(int proxyId) const;

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
	void RayCast(T* callback, const b2RayCastInput& input) const;

	/// Validate this tree. For testing.
	void Validate() const;

	/// Compute the height of the binary tree in O(N) time. Should not be
	/// called often.
	int GetHeight() const;

	/// Get the maximum balance of an node in the tree. The balance is the difference
	/// in height of the two children of a node.
	int GetMaxBalance() const;

	/// Get the ratio of the sum of the node areas to the root area.
	float GetAreaRatio() const;

	/// Build an optimal tree. Very expensive. For testing.
	void RebuildBottomUp();

	/// Shift the world origin. Useful for large worlds.
	/// The shift formula is: position -= newOrigin
	/// @param newOrigin the new origin with respect to the old origin
	void ShiftOrigin(const Vec2& newOrigin);

private:

	int AllocateNode();
	void FreeNode(int node);

	void InsertLeaf(int node);
	void RemoveLeaf(int node);

	int Balance(int index);

	int ComputeHeight() const;
	int ComputeHeight(int nodeId) const;

	void ValidateStructure(int index) const;
	void ValidateMetrics(int index) const;

	int m_root;

	std::vector<TreeNode*> m_nodes;
	int m_nodeCount;
	int m_nodeCapacity;

	int m_freeList;

	int m_insertionCount;
};

inline void* DynamicTree::GetUserData(int proxyId) const
{
	b2Assert(0 <= proxyId && proxyId < m_nodeCapacity);
	return m_nodes[proxyId]->userData;
}

inline bool DynamicTree::WasMoved(int proxyId) const
{
	b2Assert(0 <= proxyId && proxyId < m_nodeCapacity);
	return m_nodes[proxyId]->moved;
}

inline void DynamicTree::ClearMoved(int proxyId)
{
	b2Assert(0 <= proxyId && proxyId < m_nodeCapacity);
	m_nodes[proxyId]->moved = false;
}

inline const AABB& DynamicTree::GetFatAABB(int proxyId) const
{
	b2Assert(0 <= proxyId && proxyId < m_nodeCapacity);
	return m_nodes[proxyId]->aabb;
}

template <typename T>
inline void DynamicTree::Query(T* callback, const AABB& aabb) const
{
	std::vector<int> stack;
	stack.reserve(256);
	stack.push_back(m_root);

	while (stack.size() > 0)
	{
		int nodeId = stack.back();
		stack.pop_back();
		if (nodeId == b2_nullNode)
		{
			continue;
		}

		const TreeNode* node = m_nodes[nodeId];

		if (b2TestOverlap(node->aabb, aabb))
		{
			if (node->IsLeaf())
			{
				bool proceed = callback->QueryCallback(nodeId);
				if (proceed == false)
				{
					return;
				}
			}
			else
			{
				stack.push_back(node->child1);
				stack.push_back(node->child2);
			}
		}
	}
}

template <typename T>
inline void DynamicTree::RayCast(T* callback, const b2RayCastInput& input) const
{
	Vec2 p1 = input.p1;
	Vec2 p2 = input.p2;
	Vec2 r = p2 - p1;
	b2Assert(r.LengthSquared() > 0.0f);
	r.Normalize();

	// v is perpendicular to the segment.
	Vec2 v = Cross(1.0f, r);
	Vec2 abs_v = Abs(v);

	// Separating axis for segment (Gino, p80).
	// |dot(v, p1 - c)| > dot(|v|, h)

	float maxFraction = input.maxFraction;

	// Build a bounding box for the segment.
	AABB segmentAABB;
	{
		Vec2 t = p1 + maxFraction * (p2 - p1);
		segmentAABB.lowerBound = Min(p1, t);
		segmentAABB.upperBound = Max(p1, t);
	}

	std::vector<int> stack;
	stack.reserve(256);
	stack.push_back(m_root);

	while (stack.size() > 0)
	{
		int nodeId = stack.back();
		stack.pop_back();
		if (nodeId == b2_nullNode)
		{
			continue;
		}

		const TreeNode* node = m_nodes[nodeId];

		if (b2TestOverlap(node->aabb, segmentAABB) == false)
		{
			continue;
		}

		// Separating axis for segment (Gino, p80).
		// |dot(v, p1 - c)| > dot(|v|, h)
		Vec2 c = node->aabb.GetCenter();
		Vec2 h = node->aabb.GetExtents();
		float separation = Abs(Dot(v, p1 - c)) - Dot(abs_v, h);
		if (separation > 0.0f)
		{
			continue;
		}

		if (node->IsLeaf())
		{
			b2RayCastInput subInput;
			subInput.p1 = input.p1;
			subInput.p2 = input.p2;
			subInput.maxFraction = maxFraction;

			float value = callback->rayCastCallback(subInput, nodeId);

			if (value == 0.0f)
			{
				// The client has terminated the ray cast.
				return;
			}

			if (value > 0.0f)
			{
				// Update segment bounding box.
				maxFraction = value;
				Vec2 t = p1 + maxFraction * (p2 - p1);
				segmentAABB.lowerBound = Min(p1, t);
				segmentAABB.upperBound = Max(p1, t);
			}
		}
		else
		{
			stack.push_back(node->child1);
			stack.push_back(node->child2);
		}
	}
}
