#include "Body.h"
#include "Contact.h"
#include "ContactManager.h"
#include "Fixture.h"
#include "World.h"

class BroadPhase;

Body::Body(const BodyDef* bd, World* world)
{
	assert(bd->position.IsValid());
	assert(bd->linearVelocity.IsValid());
	assert(isfinite(bd->angle));
	assert(isfinite(bd->angularVelocity));
	assert(isfinite(bd->angularDamping) && bd->angularDamping >= 0.0f);
	assert(isfinite(bd->linearDamping) && bd->linearDamping >= 0.0f);

	m_flags = 0;

	if (bd->bullet)
	{
		m_flags |= e_bulletFlag;
	}
	if (bd->fixedRotation)
	{
		m_flags |= e_fixedRotationFlag;
	}
	if (bd->allowSleep)
	{
		m_flags |= e_autoSleepFlag;
	}
	if (bd->awake && bd->type != staticBody)
	{
		m_flags |= e_awakeFlag;
	}
	if (bd->enabled)
	{
		m_flags |= e_enabledFlag;
	}

	m_world = world;

	m_xf.p = bd->position;
	m_xf.q.Set(bd->angle);

	m_sweep.localCenter.SetZero();
	m_sweep.c0 = m_xf.p;
	m_sweep.c = m_xf.p;
	m_sweep.a0 = bd->angle;
	m_sweep.a = bd->angle;
	m_sweep.alpha0 = 0.0f;

	m_contactList = {};
	m_prev = nullptr;
	m_next = nullptr;

	m_linearVelocity = bd->linearVelocity;
	m_angularVelocity = bd->angularVelocity;

	m_linearDamping = bd->linearDamping;
	m_angularDamping = bd->angularDamping;
	m_gravityScale = bd->gravityScale;

	m_force.SetZero();
	m_torque = 0.0f;

	m_sleepTime = 0.0f;

	m_type = bd->type;

	m_mass = 0.0f;
	m_invMass = 0.0f;

	m_I = 0.0f;
	m_invI = 0.0f;

	m_fixtureList = {};
	m_fixtureCount = 0;
}

Body::~Body()
{
}

void Body::SetType(BodyType type)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return;
	}

	if (m_type == type)
	{
		return;
	}

	m_type = type;

	ResetMassData();

	if (m_type == staticBody)
	{
		m_linearVelocity.SetZero();
		m_angularVelocity = 0.0f;
		m_sweep.a0 = m_sweep.a;
		m_sweep.c0 = m_sweep.c;
		m_flags &= ~e_awakeFlag;
		SynchronizeFixtures();
	}

	SetAwake(true);

	m_force.SetZero();
	m_torque = 0.0f;

	// Delete the attached contacts.
	ContactEdge* ce = m_contactList;
	while (ce)
	{
		ContactEdge* ce0 = ce;
		ce = ce->next;
		m_world->m_contactManager.Destroy(ce0->contact);
	}
	m_contactList = nullptr;

	// Touch the proxies so that new contacts will be created (when appropriate)
	BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
	for (Fixture* f = m_fixtureList; f; f = f->m_next)
	{
		int proxyCount = f->m_proxyCount;
		for (int i = 0; i < proxyCount; ++i)
		{
			broadPhase->TouchProxy(f->m_proxies[i]->proxyId);
		}
	}
}

Fixture* Body::CreateFixture(const FixtureDef* def)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return nullptr;
	}

	Fixture* fixture = new Fixture;
	fixture->Create(this, def);

	if (m_flags & e_enabledFlag)
	{
		BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
		fixture->CreateProxies(broadPhase, m_xf);
	}

	fixture->m_next = m_fixtureList;
	m_fixtureList = fixture;
	++m_fixtureCount;

	fixture->m_body = this;

	// Adjust mass properties if needed.
	if (fixture->m_density > 0.0f)
	{
		ResetMassData();
	}

	// Let the world know we have a new fixture. This will cause new contacts
	// to be created at the beginning of the next time step.
	m_world->m_newContacts = true;

	return fixture;
}

Fixture* Body::CreateFixture(const Shape* shape, float density)
{
	FixtureDef def;
	def.shape = shape;
	def.density = density;

	return CreateFixture(&def);
}

void Body::DestroyFixture(Fixture* fixture)
{
	if (fixture == nullptr)
	{
		return;
	}

	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return;
	}

	assert(fixture->m_body == this);

	// Remove the fixture from this body's singly linked list.
	assert(m_fixtureCount > 0);
	Fixture* node = m_fixtureList;
	bool found = false;
	while (node)
	{
		if (node == fixture)
		{
			node = fixture->m_next;
			found = true;
			break;
		}

		node = node->m_next;
	}

	// You tried to remove a shape that is not attached to this body.
	assert(found);

	const float density = fixture->m_density;

	// Destroy any contacts associated with the fixture.
	ContactEdge* edge = m_contactList;
	while (edge)
	{
		Contact* c = edge->contact;
		edge = edge->next;

		Fixture* fixtureA = c->GetFixtureA();
		Fixture* fixtureB = c->GetFixtureB();

		if (fixture == fixtureA || fixture == fixtureB)
		{
			// This destroys the contact and removes it from
			// this body's contact list.
			m_world->m_contactManager.Destroy(c);
		}
	}

	if (m_flags & e_enabledFlag)
	{
		BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
		fixture->DestroyProxies(broadPhase);
	}

	fixture->m_body = nullptr;
	fixture->m_next = nullptr;
	fixture->~Fixture();

	--m_fixtureCount;

	// Reset the mass data
	if (density > 0.0f)
	{
		ResetMassData();
	}
}

void Body::ResetMassData()
{
	// Compute mass data from shapes. Each shape has its own density.
	m_mass = 0.0f;
	m_invMass = 0.0f;
	m_I = 0.0f;
	m_invI = 0.0f;
	m_sweep.localCenter.SetZero();

	// Static and kinematic bodies have zero mass.
	if (m_type == staticBody || m_type == kinematicBody)
	{
		m_sweep.c0 = m_xf.p;
		m_sweep.c = m_xf.p;
		m_sweep.a0 = m_sweep.a;
		return;
	}

	assert(m_type == dynamicBody);

	// Accumulate mass over all fixtures.
	Vec2 localCenter = Vec2_zero;
	for (Fixture* f = m_fixtureList; f; f = f->m_next)
	{
		if (f->m_density == 0.0f)
		{
			continue;
		}

		MassData massData;
		f->GetMassData(&massData);
		m_mass += massData.mass;
		localCenter += massData.mass * massData.center;
		m_I += massData.I;
	}

	// Compute center of mass.
	if (m_mass > 0.0f)
	{
		m_invMass = 1.0f / m_mass;
		localCenter *= m_invMass;
	}

	if (m_I > 0.0f && (m_flags & e_fixedRotationFlag) == 0)
	{
		// Center the inertia about the center of mass.
		m_I -= m_mass * Dot(localCenter, localCenter);
		assert(m_I > 0.0f);
		m_invI = 1.0f / m_I;

	}
	else
	{
		m_I = 0.0f;
		m_invI = 0.0f;
	}

	// Move center of mass.
	Vec2 oldCenter = m_sweep.c;
	m_sweep.localCenter = localCenter;
	m_sweep.c0 = m_sweep.c = Mul(m_xf, m_sweep.localCenter);

	// Update center of mass velocity.
	m_linearVelocity += Cross(m_angularVelocity, m_sweep.c - oldCenter);
}

void Body::SetMassData(const MassData* massData)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return;
	}

	if (m_type != dynamicBody)
	{
		return;
	}

	m_invMass = 0.0f;
	m_I = 0.0f;
	m_invI = 0.0f;

	m_mass = massData->mass;
	if (m_mass <= 0.0f)
	{
		m_mass = 1.0f;
	}

	m_invMass = 1.0f / m_mass;

	if (massData->I > 0.0f && (m_flags & Body::e_fixedRotationFlag) == 0)
	{
		m_I = massData->I - m_mass * Dot(massData->center, massData->center);
		assert(m_I > 0.0f);
		m_invI = 1.0f / m_I;
	}

	// Move center of mass.
	Vec2 oldCenter = m_sweep.c;
	m_sweep.localCenter =  massData->center;
	m_sweep.c0 = m_sweep.c = Mul(m_xf, m_sweep.localCenter);

	// Update center of mass velocity.
	m_linearVelocity += Cross(m_angularVelocity, m_sweep.c - oldCenter);
}

bool Body::ShouldCollide(const Body* other) const
{
	// At least one body should be dynamic.
	if (m_type != dynamicBody && other->m_type != dynamicBody)
	{
		return false;
	}

	return true;
}

void Body::SetTransform(const Vec2& position, float angle)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return;
	}

	m_xf.q.Set(angle);
	m_xf.p = position;

	m_sweep.c = Mul(m_xf, m_sweep.localCenter);
	m_sweep.a = angle;

	m_sweep.c0 = m_sweep.c;
	m_sweep.a0 = angle;

	BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
	for (Fixture* f = m_fixtureList; f; f = f->m_next)
	{
		f->Synchronize(broadPhase, m_xf, m_xf);
	}

	// Check for new contacts the next step
	m_world->m_newContacts = true;
}

void Body::SynchronizeFixtures()
{
	BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
	if (m_flags & Body::e_awakeFlag)
	{
		Transform xf1;
		xf1.q.Set(m_sweep.a0);
		xf1.p = m_sweep.c0 - Mul(xf1.q, m_sweep.localCenter);

		for (Fixture* f = m_fixtureList; f; f = f->m_next)
		{
			f->Synchronize(broadPhase, xf1, m_xf);
		}
	}
	else
	{
		for (Fixture* f = m_fixtureList; f; f = f->m_next)
		{
			f->Synchronize(broadPhase, m_xf, m_xf);
		}
	}
}

void Body::SetEnabled(bool flag)
{
	assert(m_world->IsLocked() == false);

	if (flag == IsEnabled())
	{
		return;
	}

	if (flag)
	{
		m_flags |= e_enabledFlag;

		// Create all proxies.
		BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
		for (Fixture* f = m_fixtureList; f; f = f->m_next)
		{
			f->CreateProxies(broadPhase, m_xf);
		}

		// Contacts are created at the beginning of the next
		m_world->m_newContacts = true;
	}
	else
	{
		m_flags &= ~e_enabledFlag;

		// Destroy all proxies.
		BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
		for (Fixture* f = m_fixtureList; f; f = f->m_next)
		{
			f->DestroyProxies(broadPhase);
		}

		// Destroy the attached contacts.
		ContactEdge* ce = m_contactList;
		while (ce)
		{
			ContactEdge* ce0 = ce;
			ce = ce->next;
			m_world->m_contactManager.Destroy(ce0->contact);
		}
		m_contactList = nullptr;
	}
}

void Body::SetFixedRotation(bool flag)
{
	bool status = (m_flags & e_fixedRotationFlag) == e_fixedRotationFlag;
	if (status == flag)
	{
		return;
	}

	if (flag)
	{
		m_flags |= e_fixedRotationFlag;
	}
	else
	{
		m_flags &= ~e_fixedRotationFlag;
	}

	m_angularVelocity = 0.0f;

	ResetMassData();
}
