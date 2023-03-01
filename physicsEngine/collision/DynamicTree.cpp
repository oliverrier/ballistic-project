#include "DynamicTree.h"

#include "../common/Math.h"

DynamicTree::DynamicTree()
{
	m_root = nullNode;

	m_nodeCapacity = 16;
	m_nodeCount = 0;
	m_nodes = std::vector<TreeNode*>{};
	m_nodes.resize(m_nodeCapacity);

	// Build a linked list for the free list.
	for (int i = 0; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i] = new TreeNode;
		m_nodes[i]->next = i + 1;
		m_nodes[i]->height = -1;
	}
	m_nodes[m_nodeCapacity - 1] = new TreeNode;
	m_nodes[m_nodeCapacity-1]->next = nullNode;
	m_nodes[m_nodeCapacity-1]->height = -1;
	m_freeList = 0;

	m_insertionCount = 0;
}

DynamicTree::~DynamicTree()
{
	// This frees the entire tree in one shot.
	m_nodes.clear();
}

// Allocate a node from the pool. Grow the pool if necessary.
int DynamicTree::AllocateNode()
{
	// Expand the node pool as needed.
	if (m_freeList == nullNode)
	{
		assert(m_nodeCount == m_nodeCapacity);

		// The free list is empty. Rebuild a bigger pool.
		m_nodeCapacity *= 2;
		m_nodes.resize(m_nodeCapacity);

		// Build a linked list for the free list. The parent
		// pointer becomes the "next" pointer.
		for (int i = m_nodeCount; i < m_nodeCapacity - 1; ++i)
		{
			m_nodes[i] = new TreeNode;
			m_nodes[i]->next = i + 1;
			m_nodes[i]->height = -1;
		}
		m_nodes[m_nodeCapacity - 1] = new TreeNode;
		m_nodes[m_nodeCapacity-1]->next = nullNode;
		m_nodes[m_nodeCapacity-1]->height = -1;
		m_freeList = m_nodeCount;
	}

	// Peel a node off the free list.
	int nodeId = m_freeList;
	m_freeList = m_nodes[nodeId]->next;
	m_nodes[nodeId]->parent = nullNode;
	m_nodes[nodeId]->child1 = nullNode;
	m_nodes[nodeId]->child2 = nullNode;
	m_nodes[nodeId]->height = 0;
	m_nodes[nodeId]->userData = nullptr;
	m_nodes[nodeId]->moved = false;
	++m_nodeCount;
	return nodeId;
}

// Return a node to the pool.
void DynamicTree::FreeNode(int nodeId)
{
	assert(0 <= nodeId && nodeId < m_nodeCapacity);
	assert(0 < m_nodeCount);
	m_nodes[nodeId]->next = m_freeList;
	m_nodes[nodeId]->height = -1;
	m_freeList = nodeId;
	--m_nodeCount;
}

// Create a proxy in the tree as a leaf node. We return the index
// of the node instead of a pointer so that we can grow
// the node pool.
int DynamicTree::CreateProxy(const AABB& aabb, void* userData)
{
	int proxyId = AllocateNode();

	// Fatten the aabb.
	Vec2 r(aabbExtension, aabbExtension);
	m_nodes[proxyId]->aabb.lowerBound = aabb.lowerBound - r;
	m_nodes[proxyId]->aabb.upperBound = aabb.upperBound + r;
	m_nodes[proxyId]->userData = userData;
	m_nodes[proxyId]->height = 0;
	m_nodes[proxyId]->moved = true;

	InsertLeaf(proxyId);

	return proxyId;
}

void DynamicTree::DestroyProxy(int proxyId)
{
	assert(0 <= proxyId && proxyId < m_nodeCapacity);
	assert(m_nodes[proxyId]->IsLeaf());

	RemoveLeaf(proxyId);
	FreeNode(proxyId);
}

bool DynamicTree::MoveProxy(int proxyId, const AABB& aabb, const Vec2& displacement)
{
	assert(0 <= proxyId && proxyId < m_nodeCapacity);

	assert(m_nodes[proxyId]->IsLeaf());

	// Extend AABB
	AABB fatAABB;
	Vec2 r(aabbExtension, aabbExtension);
	fatAABB.lowerBound = aabb.lowerBound - r;
	fatAABB.upperBound = aabb.upperBound + r;

	// Predict AABB movement
	Vec2 d = aabbMultiplier * displacement;

	if (d.x < 0.0f)
	{
		fatAABB.lowerBound.x += d.x;
	}
	else
	{
		fatAABB.upperBound.x += d.x;
	}

	if (d.y < 0.0f)
	{
		fatAABB.lowerBound.y += d.y;
	}
	else
	{
		fatAABB.upperBound.y += d.y;
	}

	const AABB& treeAABB = m_nodes[proxyId]->aabb;
	if (treeAABB.Contains(aabb))
	{
		// The tree AABB still contains the object, but it might be too large.
		// Perhaps the object was moving fast but has since gone to sleep.
		// The huge AABB is larger than the new fat AABB.
		AABB hugeAABB;
		hugeAABB.lowerBound = fatAABB.lowerBound - 4.0f * r;
		hugeAABB.upperBound = fatAABB.upperBound + 4.0f * r;

		if (hugeAABB.Contains(treeAABB))
		{
			// The tree AABB contains the object AABB and the tree AABB is
			// not too large. No tree update needed.
			return false;
		}

		// Otherwise the tree AABB is huge and needs to be shrunk
	}

	RemoveLeaf(proxyId);

	m_nodes[proxyId]->aabb = fatAABB;

	InsertLeaf(proxyId);

	m_nodes[proxyId]->moved = true;

	return true;
}

void DynamicTree::InsertLeaf(int leaf)
{
	++m_insertionCount;

	if (m_root == nullNode)
	{
		m_root = leaf;
		m_nodes[m_root]->parent = nullNode;
		return;
	}

	// Find the best sibling for this node
	AABB leafAABB = m_nodes[leaf]->aabb;
	int index = m_root;
	while (m_nodes[index]->IsLeaf() == false)
	{
		int child1 = m_nodes[index]->child1;
		int child2 = m_nodes[index]->child2;

		float area = m_nodes[index]->aabb.GetPerimeter();

		AABB combinedAABB;
		combinedAABB.Combine(m_nodes[index]->aabb, leafAABB);
		float combinedArea = combinedAABB.GetPerimeter();

		// Cost of creating a new parent for this node and the new leaf
		float cost = 2.0f * combinedArea;

		// Minimum cost of pushing the leaf further down the tree
		float inheritanceCost = 2.0f * (combinedArea - area);

		// Cost of descending into child1
		float cost1;
		if (m_nodes[child1]->IsLeaf())
		{
			AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child1]->aabb);
			cost1 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child1]->aabb);
			float oldArea = m_nodes[child1]->aabb.GetPerimeter();
			float newArea = aabb.GetPerimeter();
			cost1 = (newArea - oldArea) + inheritanceCost;
		}

		// Cost of descending into child2
		float cost2;
		if (m_nodes[child2]->IsLeaf())
		{
			AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child2]->aabb);
			cost2 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child2]->aabb);
			float oldArea = m_nodes[child2]->aabb.GetPerimeter();
			float newArea = aabb.GetPerimeter();
			cost2 = newArea - oldArea + inheritanceCost;
		}

		// Descend according to the minimum cost.
		if (cost < cost1 && cost < cost2)
		{
			break;
		}

		// Descend
		if (cost1 < cost2)
		{
			index = child1;
		}
		else
		{
			index = child2;
		}
	}

	int sibling = index;

	// Create a new parent.
	int oldParent = m_nodes[sibling]->parent;
	int newParent = AllocateNode();
	m_nodes[newParent]->parent = oldParent;
	m_nodes[newParent]->userData = nullptr;
	m_nodes[newParent]->aabb.Combine(leafAABB, m_nodes[sibling]->aabb);
	m_nodes[newParent]->height = m_nodes[sibling]->height + 1;

	if (oldParent != nullNode)
	{
		// The sibling was not the root.
		if (m_nodes[oldParent]->child1 == sibling)
		{
			m_nodes[oldParent]->child1 = newParent;
		}
		else
		{
			m_nodes[oldParent]->child2 = newParent;
		}

		m_nodes[newParent]->child1 = sibling;
		m_nodes[newParent]->child2 = leaf;
		m_nodes[sibling]->parent = newParent;
		m_nodes[leaf]->parent = newParent;
	}
	else
	{
		// The sibling was the root.
		m_nodes[newParent]->child1 = sibling;
		m_nodes[newParent]->child2 = leaf;
		m_nodes[sibling]->parent = newParent;
		m_nodes[leaf]->parent = newParent;
		m_root = newParent;
	}

	// Walk back up the tree fixing heights and AABBs
	index = m_nodes[leaf]->parent;
	while (index != nullNode)
	{
		index = Balance(index);

		int child1 = m_nodes[index]->child1;
		int child2 = m_nodes[index]->child2;

		assert(child1 != nullNode);
		assert(child2 != nullNode);

		m_nodes[index]->height = 1 + Max(m_nodes[child1]->height, m_nodes[child2]->height);
		m_nodes[index]->aabb.Combine(m_nodes[child1]->aabb, m_nodes[child2]->aabb);

		index = m_nodes[index]->parent;
	}

	//Validate();
}

void DynamicTree::RemoveLeaf(int leaf)
{
	if (leaf == m_root)
	{
		m_root = nullNode;
		return;
	}

	int parent = m_nodes[leaf]->parent;
	int grandParent = m_nodes[parent]->parent;
	int sibling;
	if (m_nodes[parent]->child1 == leaf)
	{
		sibling = m_nodes[parent]->child2;
	}
	else
	{
		sibling = m_nodes[parent]->child1;
	}

	if (grandParent != nullNode)
	{
		// Destroy parent and connect sibling to grandParent.
		if (m_nodes[grandParent]->child1 == parent)
		{
			m_nodes[grandParent]->child1 = sibling;
		}
		else
		{
			m_nodes[grandParent]->child2 = sibling;
		}
		m_nodes[sibling]->parent = grandParent;
		FreeNode(parent);

		// Adjust ancestor bounds.
		int index = grandParent;
		while (index != nullNode)
		{
			index = Balance(index);

			int child1 = m_nodes[index]->child1;
			int child2 = m_nodes[index]->child2;

			m_nodes[index]->aabb.Combine(m_nodes[child1]->aabb, m_nodes[child2]->aabb);
			m_nodes[index]->height = 1 + Max(m_nodes[child1]->height, m_nodes[child2]->height);

			index = m_nodes[index]->parent;
		}
	}
	else
	{
		m_root = sibling;
		m_nodes[sibling]->parent = nullNode;
		FreeNode(parent);
	}

	//Validate();
}

// Perform a left or right rotation if node A is imbalanced.
// Returns the new root index.
int DynamicTree::Balance(int iA)
{
	assert(iA != nullNode);

	TreeNode* A = m_nodes[iA];
	if (A->IsLeaf() || A->height < 2)
	{
		return iA;
	}

	int iB = A->child1;
	int iC = A->child2;
	assert(0 <= iB && iB < m_nodeCapacity);
	assert(0 <= iC && iC < m_nodeCapacity);

	TreeNode* B = m_nodes[iB];
	TreeNode* C = m_nodes[iC];

	int balance = C->height - B->height;

	// Rotate C up
	if (balance > 1)
	{
		int iF = C->child1;
		int iG = C->child2;
		TreeNode* F = m_nodes[iF];
		TreeNode* G = m_nodes[iG];
		assert(0 <= iF && iF < m_nodeCapacity);
		assert(0 <= iG && iG < m_nodeCapacity);

		// Swap A and C
		C->child1 = iA;
		C->parent = A->parent;
		A->parent = iC;

		// A's old parent should point to C
		if (C->parent != nullNode)
		{
			if (m_nodes[C->parent]->child1 == iA)
			{
				m_nodes[C->parent]->child1 = iC;
			}
			else
			{
				assert(m_nodes[C->parent]->child2 == iA);
				m_nodes[C->parent]->child2 = iC;
			}
		}
		else
		{
			m_root = iC;
		}

		// Rotate
		if (F->height > G->height)
		{
			C->child2 = iF;
			A->child2 = iG;
			G->parent = iA;
			A->aabb.Combine(B->aabb, G->aabb);
			C->aabb.Combine(A->aabb, F->aabb);

			A->height = 1 + Max(B->height, G->height);
			C->height = 1 + Max(A->height, F->height);
		}
		else
		{
			C->child2 = iG;
			A->child2 = iF;
			F->parent = iA;
			A->aabb.Combine(B->aabb, F->aabb);
			C->aabb.Combine(A->aabb, G->aabb);

			A->height = 1 + Max(B->height, F->height);
			C->height = 1 + Max(A->height, G->height);
		}

		return iC;
	}
	
	// Rotate B up
	if (balance < -1)
	{
		int iD = B->child1;
		int iE = B->child2;
		TreeNode* D = m_nodes[iD];
		TreeNode* E = m_nodes[iE];
		assert(0 <= iD && iD < m_nodeCapacity);
		assert(0 <= iE && iE < m_nodeCapacity);

		// Swap A and B
		B->child1 = iA;
		B->parent = A->parent;
		A->parent = iB;

		// A's old parent should point to B
		if (B->parent != nullNode)
		{
			if (m_nodes[B->parent]->child1 == iA)
			{
				m_nodes[B->parent]->child1 = iB;
			}
			else
			{
				assert(m_nodes[B->parent]->child2 == iA);
				m_nodes[B->parent]->child2 = iB;
			}
		}
		else
		{
			m_root = iB;
		}

		// Rotate
		if (D->height > E->height)
		{
			B->child2 = iD;
			A->child1 = iE;
			E->parent = iA;
			A->aabb.Combine(C->aabb, E->aabb);
			B->aabb.Combine(A->aabb, D->aabb);

			A->height = 1 + Max(C->height, E->height);
			B->height = 1 + Max(A->height, D->height);
		}
		else
		{
			B->child2 = iE;
			A->child1 = iD;
			D->parent = iA;
			A->aabb.Combine(C->aabb, D->aabb);
			B->aabb.Combine(A->aabb, E->aabb);

			A->height = 1 + Max(C->height, D->height);
			B->height = 1 + Max(A->height, E->height);
		}

		return iB;
	}

	return iA;
}

int DynamicTree::GetHeight() const
{
	if (m_root == nullNode)
	{
		return 0;
	}

	return m_nodes[m_root]->height;
}

//
float DynamicTree::GetAreaRatio() const
{
	if (m_root == nullNode)
	{
		return 0.0f;
	}

	const TreeNode* root = m_nodes[m_root];
	float rootArea = root->aabb.GetPerimeter();

	float totalArea = 0.0f;
	for (int i = 0; i < m_nodeCapacity; ++i)
	{
		const TreeNode* node = m_nodes[i];
		if (node->height < 0)
		{
			// Free node in pool
			continue;
		}

		totalArea += node->aabb.GetPerimeter();
	}

	return totalArea / rootArea;
}

// Compute the height of a sub-tree.
int DynamicTree::ComputeHeight(int nodeId) const
{
	assert(0 <= nodeId && nodeId < m_nodeCapacity);
	TreeNode* node = m_nodes[nodeId];

	if (node->IsLeaf())
	{
		return 0;
	}

	int height1 = ComputeHeight(node->child1);
	int height2 = ComputeHeight(node->child2);
	return 1 + Max(height1, height2);
}

int DynamicTree::ComputeHeight() const
{
	int height = ComputeHeight(m_root);
	return height;
}

void DynamicTree::ValidateStructure(int index) const
{
	if (index == nullNode)
	{
		return;
	}

	if (index == m_root)
	{
		assert(m_nodes[index]->parent == nullNode);
	}

	const TreeNode* node = m_nodes[index];

	int child1 = node->child1;
	int child2 = node->child2;

	if (node->IsLeaf())
	{
		assert(child1 == nullNode);
		assert(child2 == nullNode);
		assert(node->height == 0);
		return;
	}

	assert(0 <= child1 && child1 < m_nodeCapacity);
	assert(0 <= child2 && child2 < m_nodeCapacity);

	assert(m_nodes[child1]->parent == index);
	assert(m_nodes[child2]->parent == index);

	ValidateStructure(child1);
	ValidateStructure(child2);
}

void DynamicTree::ValidateMetrics(int index) const
{
	if (index == nullNode)
	{
		return;
	}

	const TreeNode* node = m_nodes[index];

	int child1 = node->child1;
	int child2 = node->child2;

	if (node->IsLeaf())
	{
		assert(child1 == nullNode);
		assert(child2 == nullNode);
		assert(node->height == 0);
		return;
	}

	assert(0 <= child1 && child1 < m_nodeCapacity);
	assert(0 <= child2 && child2 < m_nodeCapacity);

	int height1 = m_nodes[child1]->height;
	int height2 = m_nodes[child2]->height;
	int height;
	height = 1 + Max(height1, height2);
	assert(node->height == height);

	AABB aabb;
	aabb.Combine(m_nodes[child1]->aabb, m_nodes[child2]->aabb);

	assert(aabb.lowerBound == node->aabb.lowerBound);
	assert(aabb.upperBound == node->aabb.upperBound);

	ValidateMetrics(child1);
	ValidateMetrics(child2);
}

void DynamicTree::Validate() const
{
#if defined(DEBUG)
	ValidateStructure(m_root);
	ValidateMetrics(m_root);

	int freeCount = 0;
	int freeIndex = m_freeList;
	while (freeIndex != nullNode)
	{
		assert(0 <= freeIndex && freeIndex < m_nodeCapacity);
		freeIndex = m_nodes[freeIndex]->next;
		++freeCount;
	}

	assert(GetHeight() == ComputeHeight());

	assert(m_nodeCount + freeCount == m_nodeCapacity);
#endif
}

int DynamicTree::GetMaxBalance() const
{
	int maxBalance = 0;
	for (int i = 0; i < m_nodeCapacity; ++i)
	{
		const TreeNode* node = m_nodes[i];
		if (node->height <= 1)
		{
			continue;
		}

		assert(node->IsLeaf() == false);

		int child1 = node->child1;
		int child2 = node->child2;
		int balance = Abs(m_nodes[child2]->height - m_nodes[child1]->height);
		maxBalance = Max(maxBalance, balance);
	}

	return maxBalance;
}

void DynamicTree::RebuildBottomUp()
{
	std::vector<int> nodes(m_nodeCount);
	int count = 0;

	// Build array of leaves. Free the rest.
	for (int i = 0; i < m_nodeCapacity; ++i)
	{
		if (m_nodes[i]->height < 0)
		{
			// free node in pool
			continue;
		}

		if (m_nodes[i]->IsLeaf())
		{
			m_nodes[i]->parent = nullNode;
			nodes[count] = i;
			++count;
		}
		else
		{
			FreeNode(i);
		}
	}

	while (count > 1)
	{
		float minCost = FLT_MAX;
		int iMin = -1, jMin = -1;
		for (int i = 0; i < count; ++i)
		{
			AABB aabbi = m_nodes[nodes[i]]->aabb;

			for (int j = i + 1; j < count; ++j)
			{
				AABB aabbj = m_nodes[nodes[j]]->aabb;
				AABB b;
				b.Combine(aabbi, aabbj);
				float cost = b.GetPerimeter();
				if (cost < minCost)
				{
					iMin = i;
					jMin = j;
					minCost = cost;
				}
			}
		}

		int index1 = nodes[iMin];
		int index2 = nodes[jMin];
		TreeNode* child1 = m_nodes[index1];
		TreeNode* child2 = m_nodes[index2];

		int parentIndex = AllocateNode();
		TreeNode* parent = m_nodes[parentIndex];
		parent->child1 = index1;
		parent->child2 = index2;
		parent->height = 1 + Max(child1->height, child2->height);
		parent->aabb.Combine(child1->aabb, child2->aabb);
		parent->parent = nullNode;

		child1->parent = parentIndex;
		child2->parent = parentIndex;

		nodes[jMin] = nodes[count-1];
		nodes[iMin] = parentIndex;
		--count;
	}

	m_root = nodes[0];
	nodes.clear();

	Validate();
}

void DynamicTree::ShiftOrigin(const Vec2& newOrigin)
{
	// Build array of leaves. Free the rest.
	for (int i = 0; i < m_nodeCapacity; ++i)
	{
		m_nodes[i]->aabb.lowerBound -= newOrigin;
		m_nodes[i]->aabb.upperBound -= newOrigin;
	}
}
