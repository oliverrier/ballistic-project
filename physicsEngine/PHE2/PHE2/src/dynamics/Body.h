#pragma once
#include <vector>
#include "../common/Math.h"
#include "../collision/Shape.h"

struct MassData;
class Fixture;
class b2Joint;
class Contact;
class Controller;
class World;
struct FixtureDef;
struct b2JointEdge;
struct b2ContactEdge;

/// The body type.
/// static: zero mass, zero velocity, may be manually moved
/// kinematic: zero mass, non-zero velocity set by user, moved by solver
/// dynamic: positive mass, non-zero velocity determined by forces, moved by solver
enum b2BodyType
{
	b2_staticBody = 0,
	b2_kinematicBody,
	dynamicBody
};

/// A body definition holds all the data needed to construct a rigid body.
/// You can safely re-use body definitions. Shapes are added to a body after construction.
struct BodyDef
{
	/// This constructor sets the body definition default values.
	BodyDef()
	{
		position.Set(0.0f, 0.0f);
		angle = 0.0f;
		linearVelocity.Set(0.0f, 0.0f);
		angularVelocity = 0.0f;
		linearDamping = 0.0f;
		angularDamping = 0.0f;
		allowSleep = true;
		awake = true;
		fixedRotation = false;
		bullet = false;
		type = b2_staticBody;
		enabled = true;
		gravityScale = 1.0f;
	}

	/// The body type: static, kinematic, or dynamic.
	/// Note: if a dynamic body would have zero mass, the mass is set to one.
	b2BodyType type;

	/// The world position of the body. Avoid creating bodies at the origin
	/// since this can lead to many overlapping shapes.
	Vec2 position;

	/// The world angle of the body in radians.
	float angle;

	/// The linear velocity of the body's origin in world co-ordinates.
	Vec2 linearVelocity;

	/// The angular velocity of the body.
	float angularVelocity;

	/// Linear damping is use to reduce the linear velocity. The damping parameter
	/// can be larger than 1.0f but the damping effect becomes sensitive to the
	/// time step when the damping parameter is large.
	/// Units are 1/time
	float linearDamping;

	/// Angular damping is use to reduce the angular velocity. The damping parameter
	/// can be larger than 1.0f but the damping effect becomes sensitive to the
	/// time step when the damping parameter is large.
	/// Units are 1/time
	float angularDamping;

	/// Set this flag to false if this body should never fall asleep. Note that
	/// this increases CPU usage.
	bool allowSleep;

	/// Is this body initially awake or sleeping?
	bool awake;

	/// Should this body be prevented from rotating? Useful for characters.
	bool fixedRotation;

	/// Is this a fast moving body that should be prevented from tunneling through
	/// other moving bodies? Note that all bodies are prevented from tunneling through
	/// kinematic and static bodies. This setting is only considered on dynamic bodies.
	/// @warning You should use this flag sparingly since it increases processing time.
	bool bullet;

	/// Does this body start out enabled?
	bool enabled;

	/// Scale the gravity applied to this body.
	float gravityScale;
};

/// A rigid body. These are created via World::CreateBody.
class Body
{
public:
	/// Creates a fixture and attach it to this body. Use this function if you need
	/// to set some fixture parameters, like friction. Otherwise you can create the
	/// fixture directly from a shape.
	/// If the density is non-zero, this function automatically updates the mass of the body.
	/// Contacts are not created until the next time step.
	/// @param def the fixture definition.
	/// @warning This function is locked during callbacks.
	Fixture* CreateFixture(const FixtureDef* def);

	/// Creates a fixture from a shape and attach it to this body.
	/// This is a convenience function. Use FixtureDef if you need to set parameters
	/// like friction, restitution, user data, or filtering.
	/// If the density is non-zero, this function automatically updates the mass of the body.
	/// @param shape the shape to be cloned.
	/// @param density the shape density (set to zero for static bodies).
	/// @warning This function is locked during callbacks.
	Fixture* CreateFixture(const Shape* shape, float density);

	/// Destroy a fixture. This removes the fixture from the broad-phase and
	/// destroys all contacts associated with this fixture. This will
	/// automatically adjust the mass of the body if the body is dynamic and the
	/// fixture has positive density.
	/// All fixtures attached to a body are implicitly destroyed when the body is destroyed.
	/// @param fixture the fixture to be removed.
	/// @warning This function is locked during callbacks.
	void DestroyFixture(Fixture* fixture);

	/// Set the position of the body's origin and rotation.
	/// Manipulating a body's transform may cause non-physical behavior.
	/// Note: contacts are updated on the next call to World::Step.
	/// @param position the world position of the body's local origin.
	/// @param angle the world rotation in radians.
	void SetTransform(const Vec2& position, float angle);

	/// Get the body transform for the body's origin.
	/// @return the world transform of the body's origin.
	const Transform& GetTransform() const;

	/// Get the world body origin position.
	/// @return the world position of the body's origin.
	const Vec2& GetPosition() const;

	/// Get the angle in radians.
	/// @return the current world rotation angle in radians.
	float GetAngle() const;

	/// Get the world position of the center of mass.
	const Vec2& GetWorldCenter() const;

	/// Get the local position of the center of mass.
	const Vec2& GetLocalCenter() const;

	/// Set the linear velocity of the center of mass.
	/// @param v the new linear velocity of the center of mass.
	void SetLinearVelocity(const Vec2& v);

	/// Get the linear velocity of the center of mass.
	/// @return the linear velocity of the center of mass.
	const Vec2& GetLinearVelocity() const;

	/// Set the angular velocity.
	/// @param omega the new angular velocity in radians/second.
	void SetAngularVelocity(float omega);

	/// Get the angular velocity.
	/// @return the angular velocity in radians/second.
	float GetAngularVelocity() const;

	/// Apply a force at a world point. If the force is not
	/// applied at the center of mass, it will generate a torque and
	/// affect the angular velocity. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param point the world position of the point of application.
	/// @param wake also wake up the body
	void ApplyForce(const Vec2& force, const Vec2& point, bool wake);

	/// Apply a force to the center of mass. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param wake also wake up the body
	void ApplyForceToCenter(const Vec2& force, bool wake);

	/// Apply a torque. This affects the angular velocity
	/// without affecting the linear velocity of the center of mass.
	/// @param torque about the z-axis (out of the screen), usually in N-m.
	/// @param wake also wake up the body
	void ApplyTorque(float torque, bool wake);

	/// Apply an impulse at a point. This immediately modifies the velocity.
	/// It also modifies the angular velocity if the point of application
	/// is not at the center of mass. This wakes up the body.
	/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
	/// @param point the world position of the point of application.
	/// @param wake also wake up the body
	void ApplyLinearImpulse(const Vec2& impulse, const Vec2& point, bool wake);

	/// Apply an impulse to the center of mass. This immediately modifies the velocity.
	/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
	/// @param wake also wake up the body
	void ApplyLinearImpulseToCenter(const Vec2& impulse, bool wake);

	/// Apply an angular impulse.
	/// @param impulse the angular impulse in units of kg*m*m/s
	/// @param wake also wake up the body
	void ApplyAngularImpulse(float impulse, bool wake);

	/// Get the total mass of the body.
	/// @return the mass, usually in kilograms (kg).
	float GetMass() const;

	/// Get the rotational inertia of the body about the local origin.
	/// @return the rotational inertia, usually in kg-m^2.
	float GetInertia() const;

	/// Get the mass data of the body.
	/// @return a struct containing the mass, inertia and center of the body.
	MassData GetMassData() const;

	/// Set the mass properties to override the mass properties of the fixtures.
	/// Note that this changes the center of mass position.
	/// Note that creating or destroying fixtures can also alter the mass.
	/// This function has no effect if the body isn't dynamic.
	/// @param data the mass properties.
	void SetMassData(const MassData* data);

	/// This resets the mass properties to the sum of the mass properties of the fixtures.
	/// This normally does not need to be called unless you called SetMassData to override
	/// the mass and you later want to reset the mass.
	void ResetMassData();

	/// Get the world coordinates of a point given the local coordinates.
	/// @param localPoint a point on the body measured relative the the body's origin.
	/// @return the same point expressed in world coordinates.
	Vec2 GetWorldPoint(const Vec2& localPoint) const;

	/// Get the world coordinates of a vector given the local coordinates.
	/// @param localVector a vector fixed in the body.
	/// @return the same vector expressed in world coordinates.
	Vec2 GetWorldVector(const Vec2& localVector) const;

	/// Gets a local point relative to the body's origin given a world point.
	/// @param worldPoint a point in world coordinates.
	/// @return the corresponding local point relative to the body's origin.
	Vec2 GetLocalPoint(const Vec2& worldPoint) const;

	/// Gets a local vector given a world vector.
	/// @param worldVector a vector in world coordinates.
	/// @return the corresponding local vector.
	Vec2 GetLocalVector(const Vec2& worldVector) const;

	/// Get the world linear velocity of a world point attached to this body.
	/// @param worldPoint a point in world coordinates.
	/// @return the world velocity of a point.
	Vec2 GetLinearVelocityFromWorldPoint(const Vec2& worldPoint) const;

	/// Get the world velocity of a local point.
	/// @param localPoint a point in local coordinates.
	/// @return the world velocity of a point.
	Vec2 GetLinearVelocityFromLocalPoint(const Vec2& localPoint) const;

	/// Get the linear damping of the body.
	float GetLinearDamping() const;

	/// Set the linear damping of the body.
	void SetLinearDamping(float linearDamping);

	/// Get the angular damping of the body.
	float GetAngularDamping() const;

	/// Set the angular damping of the body.
	void SetAngularDamping(float angularDamping);

	/// Get the gravity scale of the body.
	float GetGravityScale() const;

	/// Set the gravity scale of the body.
	void SetGravityScale(float scale);

	/// Set the type of this body. This may alter the mass and velocity.
	void SetType(b2BodyType type);

	/// Get the type of this body.
	b2BodyType GetType() const;

	/// Should this body be treated like a bullet for continuous collision detection?
	void SetBullet(bool flag);

	/// Is this body treated like a bullet for continuous collision detection?
	bool IsBullet() const;

	/// You can disable sleeping on this body. If you disable sleeping, the
	/// body will be woken.
	void SetSleepingAllowed(bool flag);

	/// Is this body allowed to sleep
	bool IsSleepingAllowed() const;

	/// Set the sleep state of the body. A sleeping body has very
	/// low CPU cost.
	/// @param flag set to true to wake the body, false to put it to sleep.
	void SetAwake(bool flag);

	/// Get the sleeping state of this body.
	/// @return true if the body is awake.
	bool IsAwake() const;

	/// Allow a body to be disabled. A disabled body is not simulated and cannot
	/// be collided with or woken up.
	/// If you pass a flag of true, all fixtures will be added to the broad-phase.
	/// If you pass a flag of false, all fixtures will be removed from the
	/// broad-phase and all contacts will be destroyed.
	/// Fixtures and joints are otherwise unaffected. You may continue
	/// to create/destroy fixtures and joints on disabled bodies.
	/// Fixtures on a disabled body are implicitly disabled and will
	/// not participate in collisions, ray-casts, or queries.
	/// Joints connected to a disabled body are implicitly disabled.
	/// An diabled body is still owned by a World object and remains
	/// in the body list.
	void SetEnabled(bool flag);

	/// Get the active state of the body.
	bool IsEnabled() const;

	/// Set this body to have fixed rotation. This causes the mass
	/// to be reset.
	void SetFixedRotation(bool flag);

	/// Does this body have fixed rotation?
	bool IsFixedRotation() const;

	/// Get the list of all fixtures attached to this body.
	std::vector<Fixture*> GetFixtureList();
	const std::vector<Fixture*> GetFixtureList() const;

	/// Get the list of all contacts attached to this body.
	/// @warning this list changes during the time step and you may
	/// miss some collisions if you don't use ContactListener.
	std::vector<b2ContactEdge*> GetContactList();
	const std::vector<b2ContactEdge*> GetContactList() const;
	
	/// Get the parent world of this body.
	World* GetWorld();
	const World* GetWorld() const;

private:

	friend class World;
	friend class Island;
	friend class ContactManager;
	friend class ContactSolver;
	friend class Contact;

	friend class b2DistanceJoint;
	friend class b2FrictionJoint;
	friend class b2GearJoint;
	friend class b2MotorJoint;
	friend class b2MouseJoint;
	friend class b2PrismaticJoint;
	friend class b2PulleyJoint;
	friend class b2RevoluteJoint;
	friend class b2WeldJoint;
	friend class b2WheelJoint;

	// m_flags
	enum
	{
		e_islandFlag		= 0x0001,
		e_awakeFlag			= 0x0002,
		e_autoSleepFlag		= 0x0004,
		e_bulletFlag		= 0x0008,
		e_fixedRotationFlag	= 0x0010,
		e_enabledFlag		= 0x0020,
		e_toiFlag			= 0x0040
	};

	Body(const BodyDef* bd, World* world);
	~Body();

	void SynchronizeFixtures();
	void SynchronizeTransform();

	// This is used to prevent connected bodies from colliding.
	// It may lie, depending on the collideConnected flag.
	bool ShouldCollide(const Body* other) const;

	void Advance(float t);

	b2BodyType m_type;

	unsigned short m_flags;

	int m_islandIndex;

	Transform m_xf;		// the body origin transform
	Sweep m_sweep;		// the swept motion for CCD

	Vec2 m_linearVelocity;
	float m_angularVelocity;

	Vec2 m_force;
	float m_torque;

	World* m_world;

	std::vector<Fixture*> m_fixtureList;
	int m_fixtureCount;

	std::vector<b2JointEdge*> m_jointList;
	std::vector<b2ContactEdge*> m_contactList;

	float m_mass, m_invMass;

	// Rotational inertia about the center of mass.
	float m_I, m_invI;

	float m_linearDamping;
	float m_angularDamping;
	float m_gravityScale;

	float m_sleepTime;
};

inline b2BodyType Body::GetType() const
{
	return m_type;
}

inline const Transform& Body::GetTransform() const
{
	return m_xf;
}

inline const Vec2& Body::GetPosition() const
{
	return m_xf.p;
}

inline float Body::GetAngle() const
{
	return m_sweep.a;
}

inline const Vec2& Body::GetWorldCenter() const
{
	return m_sweep.c;
}

inline const Vec2& Body::GetLocalCenter() const
{
	return m_sweep.localCenter;
}

inline void Body::SetLinearVelocity(const Vec2& v)
{
	if (m_type == b2_staticBody)
	{
		return;
	}

	if (Dot(v,v) > 0.0f)
	{
		SetAwake(true);
	}

	m_linearVelocity = v;
}

inline const Vec2& Body::GetLinearVelocity() const
{
	return m_linearVelocity;
}

inline void Body::SetAngularVelocity(float w)
{
	if (m_type == b2_staticBody)
	{
		return;
	}

	if (w * w > 0.0f)
	{
		SetAwake(true);
	}

	m_angularVelocity = w;
}

inline float Body::GetAngularVelocity() const
{
	return m_angularVelocity;
}

inline float Body::GetMass() const
{
	return m_mass;
}

inline float Body::GetInertia() const
{
	return m_I + m_mass * Dot(m_sweep.localCenter, m_sweep.localCenter);
}

inline MassData Body::GetMassData() const
{
	MassData data;
	data.mass = m_mass;
	data.I = m_I + m_mass * Dot(m_sweep.localCenter, m_sweep.localCenter);
	data.center = m_sweep.localCenter;
	return data;
}

inline Vec2 Body::GetWorldPoint(const Vec2& localPoint) const
{
	return Mul(m_xf, localPoint);
}

inline Vec2 Body::GetWorldVector(const Vec2& localVector) const
{
	return Mul(m_xf.q, localVector);
}

inline Vec2 Body::GetLocalPoint(const Vec2& worldPoint) const
{
	return MulT(m_xf, worldPoint);
}

inline Vec2 Body::GetLocalVector(const Vec2& worldVector) const
{
	return MulT(m_xf.q, worldVector);
}

inline Vec2 Body::GetLinearVelocityFromWorldPoint(const Vec2& worldPoint) const
{
	return m_linearVelocity + Cross(m_angularVelocity, worldPoint - m_sweep.c);
}

inline Vec2 Body::GetLinearVelocityFromLocalPoint(const Vec2& localPoint) const
{
	return GetLinearVelocityFromWorldPoint(GetWorldPoint(localPoint));
}

inline float Body::GetLinearDamping() const
{
	return m_linearDamping;
}

inline void Body::SetLinearDamping(float linearDamping)
{
	m_linearDamping = linearDamping;
}

inline float Body::GetAngularDamping() const
{
	return m_angularDamping;
}

inline void Body::SetAngularDamping(float angularDamping)
{
	m_angularDamping = angularDamping;
}

inline float Body::GetGravityScale() const
{
	return m_gravityScale;
}

inline void Body::SetGravityScale(float scale)
{
	m_gravityScale = scale;
}

inline void Body::SetBullet(bool flag)
{
	if (flag)
	{
		m_flags |= e_bulletFlag;
	}
	else
	{
		m_flags &= ~e_bulletFlag;
	}
}

inline bool Body::IsBullet() const
{
	return (m_flags & e_bulletFlag) == e_bulletFlag;
}

inline void Body::SetAwake(bool flag)
{
	if (m_type == b2_staticBody)
	{
		return;
	}

	if (flag)
	{
		m_flags |= e_awakeFlag;
		m_sleepTime = 0.0f;
	}
	else
	{
		m_flags &= ~e_awakeFlag;
		m_sleepTime = 0.0f;
		m_linearVelocity.SetZero();
		m_angularVelocity = 0.0f;
		m_force.SetZero();
		m_torque = 0.0f;
	}
}

inline bool Body::IsAwake() const
{
	return (m_flags & e_awakeFlag) == e_awakeFlag;
}

inline bool Body::IsEnabled() const
{
	return (m_flags & e_enabledFlag) == e_enabledFlag;
}

inline bool Body::IsFixedRotation() const
{
	return (m_flags & e_fixedRotationFlag) == e_fixedRotationFlag;
}

inline void Body::SetSleepingAllowed(bool flag)
{
	if (flag)
	{
		m_flags |= e_autoSleepFlag;
	}
	else
	{
		m_flags &= ~e_autoSleepFlag;
		SetAwake(true);
	}
}

inline bool Body::IsSleepingAllowed() const
{
	return (m_flags & e_autoSleepFlag) == e_autoSleepFlag;
}

inline std::vector<Fixture*> Body::GetFixtureList()
{
	return m_fixtureList;
}

inline const std::vector<Fixture*> Body::GetFixtureList() const
{
	return m_fixtureList;
}


inline std::vector<b2ContactEdge*> Body::GetContactList()
{
	return m_contactList;
}

inline const std::vector<b2ContactEdge*> Body::GetContactList() const
{
	return m_contactList;
}

inline void Body::ApplyForce(const Vec2& force, const Vec2& point, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate a force if the body is sleeping.
	if (m_flags & e_awakeFlag)
	{
		m_force += force;
		m_torque += Cross(point - m_sweep.c, force);
	}
}

inline void Body::ApplyForceToCenter(const Vec2& force, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate a force if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_force += force;
	}
}

inline void Body::ApplyTorque(float torque, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate a force if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_torque += torque;
	}
}

inline void Body::ApplyLinearImpulse(const Vec2& impulse, const Vec2& point, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate velocity if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_linearVelocity += m_invMass * impulse;
		m_angularVelocity += m_invI * Cross(point - m_sweep.c, impulse);
	}
}

inline void Body::ApplyLinearImpulseToCenter(const Vec2& impulse, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate velocity if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_linearVelocity += m_invMass * impulse;
	}
}

inline void Body::ApplyAngularImpulse(float impulse, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate velocity if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_angularVelocity += m_invI * impulse;
	}
}

inline void Body::SynchronizeTransform()
{
	m_xf.q.Set(m_sweep.a);
	m_xf.p = m_sweep.c - Mul(m_xf.q, m_sweep.localCenter);
}

inline void Body::Advance(float alpha)
{
	// Advance to the new safe time. This doesn't sync the broad-phase.
	m_sweep.Advance(alpha);
	m_sweep.c = m_sweep.c0;
	m_sweep.a = m_sweep.a0;
	m_xf.q.Set(m_sweep.a);
	m_xf.p = m_sweep.c - Mul(m_xf.q, m_sweep.localCenter);
}

inline World* Body::GetWorld()
{
	return m_world;
}

inline const World* Body::GetWorld() const
{
	return m_world;
}

