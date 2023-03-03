#pragma once
#include "Body.h"

struct Velocity;
struct Position;
struct Vec2;
struct TimeStep;
class Contact;
class b2Joint;
class b2StackAllocator;
class ContactListener;
struct ContactVelocityConstraint;

/// This is an internal class.
class Island
{
public:
	Island(int bodyCapacity, int contactCapacity, ContactListener* listener);
	~Island();

	void Clear()
	{
		m_bodyCount = 0;
		m_contactCount = 0;
	}

	void Solve(const TimeStep& step, const Vec2& gravity, bool allowSleep);

	void SolveTOI(const TimeStep& subStep, int toiIndexA, int toiIndexB);

	void Add(Body* body)
	{
		b2Assert(m_bodyCount < m_bodyCapacity);
		body->m_islandIndex = m_bodyCount;
		m_bodies[m_bodyCount] = body;
		++m_bodyCount;
	}

	void Add(Contact* contact)
	{
		b2Assert(m_contactCount < m_contactCapacity);
		m_contacts[m_contactCount++] = contact;
	}

	void Report(const std::vector<ContactVelocityConstraint*>& constraints);

	ContactListener* m_listener;

	std::vector<Body*> m_bodies;
	std::vector<Contact*> m_contacts;

	std::vector<Position*> m_positions;
	std::vector<Velocity*> m_velocities;

	int m_bodyCount;
	int m_contactCount;

	int m_bodyCapacity;
	int m_contactCapacity;
};
