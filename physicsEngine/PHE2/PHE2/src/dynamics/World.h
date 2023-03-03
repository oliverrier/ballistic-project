#pragma once

#include <vector>

#include "ContactManager.h"
#include "WorldCallbacks.h"
#include "../common/Math.h"

struct TimeStep;
class ContactManager;
struct AABB;
struct BodyDef;
struct b2Color;
struct b2JointDef;
class Body;
class b2Draw;
class Fixture;
class b2Joint;

/// The world class manages all physics entities, dynamic simulation,
/// and asynchronous queries. The world also contains efficient memory
/// management facilities.
class World
{
public:
	/// Construct a world object.
	/// @param gravity the world gravity vector.
	World(const Vec2& gravity);

	/// Destruct the world. All physics entities are destroyed and all heap memory is released.
	~World();

	/// Register a destruction listener. The listener is owned by you and must
	/// remain in scope.
	void SetDestructionListener(DestructionListener* listener);

	/// Register a contact filter to provide specific control over collision.
	/// Otherwise the default filter is used (b2_defaultFilter). The listener is
	/// owned by you and must remain in scope.
	void SetContactFilter(ContactFilter* filter);

	/// Register a contact event listener. The listener is owned by you and must
	/// remain in scope.
	void SetContactListener(ContactListener* listener);

	/// Create a rigid body given a definition. No reference to the definition
	/// is retained.
	/// @warning This function is locked during callbacks.
	Body* CreateBody(const BodyDef* def);

	/// Destroy a rigid body given a definition. No reference to the definition
	/// is retained. This function is locked during callbacks.
	/// @warning This automatically deletes all associated shapes and joints.
	/// @warning This function is locked during callbacks.
	void DestroyBody(Body* body);

	/// Take a time step. This performs collision detection, integration,
	/// and constraint solution.
	/// @param timeStep the amount of time to simulate, this should not vary.
	/// @param velocityIterations for the velocity constraint solver.
	/// @param positionIterations for the position constraint solver.
	void Step(	float timeStep,
				int velocityIterations,
				int positionIterations);

	/// Manually clear the force buffer on all bodies. By default, forces are cleared automatically
	/// after each call to Step. The default behavior is modified by calling SetAutoClearForces.
	/// The purpose of this function is to support sub-stepping. Sub-stepping is often used to maintain
	/// a fixed sized time step under a variable frame-rate.
	/// When you perform sub-stepping you will disable auto clearing of forces and instead call
	/// ClearForces after all sub-steps are complete in one pass of your game loop.
	/// @see SetAutoClearForces
	void ClearForces();

	/// Query the world for all fixtures that potentially overlap the
	/// provided AABB.
	/// @param callback a user implemented callback class.
	/// @param aabb the query box.
	void QueryAABB(QueryCallback* callback, const AABB& aabb) const;

	/// Ray-cast the world for all fixtures in the path of the ray. Your callback
	/// controls whether you get the closest point, any point, or n-points.
	/// The ray-cast ignores shapes that contain the starting point.
	/// @param callback a user implemented callback class.
	/// @param point1 the ray starting point
	/// @param point2 the ray ending point
	void RayCast(RayCastCallback* callback, const Vec2& point1, const Vec2& point2) const;

	/// Get the world body list. With the returned body, use Body::GetNext to get
	/// the next body in the world list. A nullptr body indicates the end of the list.
	/// @return the head of the world body list.
	std::vector<Body*> GetBodyList();
	const std::vector<Body*> GetBodyList() const;

	/// Get the world contact list. With the returned contact, use Contact::GetNext to get
	/// the next contact in the world list. A nullptr contact indicates the end of the list.
	/// @return the head of the world contact list.
	/// @warning contacts are created and destroyed in the middle of a time step.
	/// Use ContactListener to avoid missing contacts.
	std::vector<Contact*> GetContactList();
	const std::vector<Contact*> GetContactList() const;

	/// Enable/disable sleep.
	void SetAllowSleeping(bool flag);
	bool GetAllowSleeping() const { return m_allowSleep; }

	/// Enable/disable warm starting. For testing.
	void SetWarmStarting(bool flag) { m_warmStarting = flag; }
	bool GetWarmStarting() const { return m_warmStarting; }

	/// Enable/disable continuous physics. For testing.
	void SetContinuousPhysics(bool flag) { m_continuousPhysics = flag; }
	bool GetContinuousPhysics() const { return m_continuousPhysics; }

	/// Enable/disable single stepped continuous physics. For testing.
	void SetSubStepping(bool flag) { m_subStepping = flag; }
	bool GetSubStepping() const { return m_subStepping; }

	/// Get the number of broad-phase proxies.
	int GetProxyCount() const;

	/// Get the number of bodies.
	int GetBodyCount() const;

	/// Get the number of contacts (each may have 0 or more contact points).
	int GetContactCount() const;

	/// Get the height of the dynamic tree.
	int GetTreeHeight() const;

	/// Get the balance of the dynamic tree.
	int GetTreeBalance() const;

	/// Get the quality metric of the dynamic tree. The smaller the better.
	/// The minimum is 1.
	float GetTreeQuality() const;

	/// Change the global gravity vector.
	void SetGravity(const Vec2& gravity);

	/// Get the global gravity vector.
	Vec2 GetGravity() const;

	/// Is the world locked (in the middle of a time step).
	bool IsLocked() const;

	/// Set flag to control automatic clearing of forces after each time step.
	void SetAutoClearForces(bool flag);

	/// Get the flag that controls automatic clearing of forces after each time step.
	bool GetAutoClearForces() const;

	/// Shift the world origin. Useful for large worlds.
	/// The body shift formula is: position -= newOrigin
	/// @param newOrigin the new origin with respect to the old origin
	void ShiftOrigin(const Vec2& newOrigin);

	/// Get the contact manager for testing.
	const ContactManager& GetContactManager() const;

private:

	friend class Body;
	friend class Fixture;
	friend class ContactManager;
	friend class Controller;

	World(const World&) = delete;
	void operator=(const World&) = delete;

	void Solve(const TimeStep& step);
	void SolveTOI(const TimeStep& step);


	ContactManager m_contactManager;

	std::vector<Body*> m_bodyList;

	int m_bodyCount;

	Vec2 m_gravity;
	bool m_allowSleep;

	DestructionListener* m_destructionListener;

	// This is used to compute the time step ratio to
	// support a variable time step.
	float m_inv_dt0;

	bool m_newContacts;
	bool m_locked;
	bool m_clearForces;

	// These are for debugging the solver.
	bool m_warmStarting;
	bool m_continuousPhysics;
	bool m_subStepping;

	bool m_stepComplete;
};

inline std::vector<Body*> World::GetBodyList()
{
	return m_bodyList;
}

inline const std::vector<Body*> World::GetBodyList() const
{
	return m_bodyList;
}

inline std::vector<Contact*> World::GetContactList()
{
	return m_contactManager.m_contactList;
}

inline const std::vector<Contact*> World::GetContactList() const
{
	return m_contactManager.m_contactList;
}

inline int World::GetBodyCount() const
{
	return m_bodyCount;
}

inline int World::GetContactCount() const
{
	return m_contactManager.m_contactCount;
}

inline void World::SetGravity(const Vec2& gravity)
{
	m_gravity = gravity;
}

inline Vec2 World::GetGravity() const
{
	return m_gravity;
}

inline bool World::IsLocked() const
{
	return m_locked;
}

inline void World::SetAutoClearForces(bool flag)
{
	m_clearForces = flag;
}

/// Get the flag that controls automatic clearing of forces after each time step.
inline bool World::GetAutoClearForces() const
{
	return m_clearForces;
}

inline const ContactManager& World::GetContactManager() const
{
	return m_contactManager;
}
