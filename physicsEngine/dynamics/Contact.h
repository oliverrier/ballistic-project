#pragma once
#include "Fixture.h"

struct WorldManifold;
class Body;
class Contact;
class Fixture;
class World;

class ContactListener;

/// Friction mixing law. The idea is to allow either fixture to drive the friction to zero.
/// For example, anything slides on ice.
inline float MixFriction(float friction1, float friction2)
{
	return sqrtf(friction1 * friction2);
}

/// Restitution mixing law. The idea is allow for anything to bounce off an inelastic surface.
/// For example, a superball bounces on anything.
inline float MixRestitution(float restitution1, float restitution2)
{
	return restitution1 > restitution2 ? restitution1 : restitution2;
}

/// Restitution mixing law. This picks the lowest value.
inline float MixRestitutionThreshold(float threshold1, float threshold2)
{
	return threshold1 < threshold2 ? threshold1 : threshold2;
}

typedef Contact* ContactCreateFcn(	Fixture* fixtureA, int indexA,
										Fixture* fixtureB, int indexB);
typedef void ContactDestroyFcn(Contact* contact);

struct ContactRegister
{
	ContactCreateFcn* createFcn;
	ContactDestroyFcn* destroyFcn;
	bool primary;
};

/// A contact edge is used to connect bodies and contacts together
/// in a contact graph where each body is a node and each contact
/// is an edge. A contact edge belongs to a doubly linked list
/// maintained in each attached body. Each contact has two contact
/// nodes, one for each attached body.
struct ContactEdge
{
	Body* other;			///< provides quick access to the other body attached.
	Contact* contact;		///< the contact
	ContactEdge* prev;	///< the previous contact edge in the body's contact list
	ContactEdge* next;	///< the next contact edge in the body's contact list
};

/// The class manages contact between two shapes. A contact exists for each overlapping
/// AABB in the broad-phase (except if filtered). Therefore a contact object may exist
/// that has no contact points.
class Contact
{
public:

	/// Get the contact manifold. Do not modify the manifold unless you understand the
	/// internals of Box2D.
	Manifold* GetManifold();
	const Manifold* GetManifold() const;

	/// Get the world manifold.
	void GetWorldManifold(WorldManifold* worldManifold) const;

	/// Is this contact touching?
	bool IsTouching() const;

	/// Enable/disable this contact. This can be used inside the pre-solve
	/// contact listener. The contact is only disabled for the current
	/// time step (or sub-step in continuous collisions).
	void SetEnabled(bool flag);

	/// Has this contact been disabled?
	bool IsEnabled() const;

	/// Get the next contact in the world's contact list.
	Contact* GetNext();
	const Contact* GetNext() const;

	/// Get fixture A in this contact.
	Fixture* GetFixtureA();
	const Fixture* GetFixtureA() const;

	/// Get the child primitive index for fixture A.
	int GetChildIndexA() const;

	/// Get fixture B in this contact.
	Fixture* GetFixtureB();
	const Fixture* GetFixtureB() const;

	/// Get the child primitive index for fixture B.
	int GetChildIndexB() const;

	/// Override the default friction mixture. You can call this in ContactListener::PreSolve.
	/// This value persists until set or reset.
	void SetFriction(float friction);

	/// Get the friction.
	float GetFriction() const;

	/// Reset the friction mixture to the default value.
	void ResetFriction();

	/// Override the default restitution mixture. You can call this in ContactListener::PreSolve.
	/// The value persists until you set or reset.
	void SetRestitution(float restitution);

	/// Get the restitution.
	float GetRestitution() const;

	/// Reset the restitution to the default value.
	void ResetRestitution();

	/// Override the default restitution velocity threshold mixture. You can call this in ContactListener::PreSolve.
	/// The value persists until you set or reset.
	void SetRestitutionThreshold(float threshold);

	/// Get the restitution threshold.
	float GetRestitutionThreshold() const;

	/// Reset the restitution threshold to the default value.
	void ResetRestitutionThreshold();

	/// Set the desired tangent speed for a conveyor belt behavior. In meters per second.
	void SetTangentSpeed(float speed);

	/// Get the desired tangent speed. In meters per second.
	float GetTangentSpeed() const;

	/// Evaluate this contact with your own manifold and transforms.
	virtual void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) = 0;

protected:
	friend class ContactManager;
	friend class World;
	friend class ContactSolver;
	friend class Body;
	friend class Fixture;

	// Flags stored in m_flags
	enum
	{
		// Used when crawling contact graph when forming islands.
		e_islandFlag		= 0x0001,

		// Set when the shapes are touching.
		e_touchingFlag		= 0x0002,

		// This contact can be disabled (by user)
		e_enabledFlag		= 0x0004,

		// This contact needs filtering because a fixture filter was changed.
		e_filterFlag		= 0x0008,

		// This bullet contact had a TOI event
		e_bulletHitFlag		= 0x0010,

		// This contact has a valid TOI in m_toi
		e_toiFlag			= 0x0020
	};

	/// Flag this contact for filtering. Filtering will occur the next time step.
	void FlagForFiltering();

	static void AddType(ContactCreateFcn* createFcn, ContactDestroyFcn* destroyFcn,
						Shape::Type typeA, Shape::Type typeB);
	static void InitializeRegisters();
	static Contact* Create(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB);
	static void Destroy(Contact* contact, Shape::Type typeA, Shape::Type typeB);
	static void Destroy(Contact* contact);

	Contact() : m_fixtureA(nullptr), m_fixtureB(nullptr) {}
	Contact(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB);
	virtual ~Contact() {}

	void Update(ContactListener* listener);

	static ContactRegister s_registers[Shape::e_typeCount][Shape::e_typeCount];
	static bool s_initialized;

	unsigned int m_flags;

	// World pool and list pointers.
	Contact* m_prev;
	Contact* m_next;

	// Nodes for connecting bodies.
	ContactEdge m_nodeA;
	ContactEdge m_nodeB;

	Fixture* m_fixtureA;
	Fixture* m_fixtureB;

	int m_indexA;
	int m_indexB;

	Manifold m_manifold;

	int m_toiCount;
	float m_toi;

	float m_friction;
	float m_restitution;
	float m_restitutionThreshold;

	float m_tangentSpeed;
};

inline Manifold* Contact::GetManifold()
{
	return &m_manifold;
}

inline const Manifold* Contact::GetManifold() const
{
	return &m_manifold;
}

inline void Contact::GetWorldManifold(WorldManifold* worldManifold) const
{
	const Body* bodyA = m_fixtureA->GetBody();
	const Body* bodyB = m_fixtureB->GetBody();
	const Shape* shapeA = m_fixtureA->GetShape();
	const Shape* shapeB = m_fixtureB->GetShape();

	worldManifold->Initialize(&m_manifold, bodyA->GetTransform(), shapeA->m_radius, bodyB->GetTransform(), shapeB->m_radius);
}

inline void Contact::SetEnabled(bool flag)
{
	if (flag)
	{
		m_flags |= e_enabledFlag;
	}
	else
	{
		m_flags &= ~e_enabledFlag;
	}
}

inline bool Contact::IsEnabled() const
{
	return (m_flags & e_enabledFlag) == e_enabledFlag;
}

inline bool Contact::IsTouching() const
{
	return (m_flags & e_touchingFlag) == e_touchingFlag;
}

inline Contact* Contact::GetNext()
{
	return m_next;
}

inline const Contact* Contact::GetNext() const
{
	return m_next;
}

inline Fixture* Contact::GetFixtureA()
{
	return m_fixtureA;
}

inline const Fixture* Contact::GetFixtureA() const
{
	return m_fixtureA;
}

inline Fixture* Contact::GetFixtureB()
{
	return m_fixtureB;
}

inline int Contact::GetChildIndexA() const
{
	return m_indexA;
}

inline const Fixture* Contact::GetFixtureB() const
{
	return m_fixtureB;
}

inline int Contact::GetChildIndexB() const
{
	return m_indexB;
}

inline void Contact::FlagForFiltering()
{
	m_flags |= e_filterFlag;
}

inline void Contact::SetFriction(float friction)
{
	m_friction = friction;
}

inline float Contact::GetFriction() const
{
	return m_friction;
}

inline void Contact::ResetFriction()
{
	m_friction = MixFriction(m_fixtureA->m_friction, m_fixtureB->m_friction);
}

inline void Contact::SetRestitution(float restitution)
{
	m_restitution = restitution;
}

inline float Contact::GetRestitution() const
{
	return m_restitution;
}

inline void Contact::ResetRestitution()
{
	m_restitution = MixRestitution(m_fixtureA->m_restitution, m_fixtureB->m_restitution);
}

inline void Contact::SetRestitutionThreshold(float threshold)
{
	m_restitutionThreshold = threshold;
}

inline float Contact::GetRestitutionThreshold() const
{
	return m_restitutionThreshold;
}

inline void Contact::ResetRestitutionThreshold()
{
	m_restitutionThreshold = MixRestitutionThreshold(m_fixtureA->m_restitutionThreshold, m_fixtureB->m_restitutionThreshold);
}

inline void Contact::SetTangentSpeed(float speed)
{
	m_tangentSpeed = speed;
}

inline float Contact::GetTangentSpeed() const
{
	return m_tangentSpeed;
}
