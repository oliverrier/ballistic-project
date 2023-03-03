#include "Contact.h"
#include "PolygonAndCircleContact.h"
#include "PolygonContact.h"
#include "Body.h"
#include "ChainAndCircleContact.h"
#include "ChainAndPolygonContact.h"
#include "CircleContact.h"
#include "EdgeAndCircleContact.h"
#include "EdgeAndPolygonContact.h"
#include "WorldCallbacks.h"


b2ContactRegister Contact::s_registers[Shape::e_typeCount][Shape::e_typeCount];
bool Contact::s_initialized = false;

void Contact::InitializeRegisters()
{
	AddType(CircleContact::Create, CircleContact::Destroy, Shape::e_circle, Shape::e_circle);
	AddType(PolygonAndCircleContact::Create, PolygonAndCircleContact::Destroy, Shape::e_polygon, Shape::e_circle);
	AddType(PolygonContact::Create, PolygonContact::Destroy, Shape::e_polygon, Shape::e_polygon);
	AddType(EdgeAndCircleContact::Create, EdgeAndCircleContact::Destroy, Shape::e_edge, Shape::e_circle);
	AddType(EdgeAndPolygonContact::Create, EdgeAndPolygonContact::Destroy, Shape::e_edge, Shape::e_polygon);
	AddType(ChainAndCircleContact::Create, ChainAndCircleContact::Destroy, Shape::e_chain, Shape::e_circle);
	AddType(ChainAndPolygonContact::Create, ChainAndPolygonContact::Destroy, Shape::e_chain, Shape::e_polygon);
}

void Contact::AddType(b2ContactCreateFcn* createFcn, b2ContactDestroyFcn* destoryFcn,
						Shape::Type type1, Shape::Type type2)
{
	b2Assert(0 <= type1 && type1 < Shape::e_typeCount);
	b2Assert(0 <= type2 && type2 < Shape::e_typeCount);
	
	s_registers[type1][type2].createFcn = createFcn;
	s_registers[type1][type2].destroyFcn = destoryFcn;
	s_registers[type1][type2].primary = true;

	if (type1 != type2)
	{
		s_registers[type2][type1].createFcn = createFcn;
		s_registers[type2][type1].destroyFcn = destoryFcn;
		s_registers[type2][type1].primary = false;
	}
}

Contact* Contact::Create(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB)
{
	if (s_initialized == false)
	{
		InitializeRegisters();
		s_initialized = true;
	}

	Shape::Type type1 = fixtureA->GetType();
	Shape::Type type2 = fixtureB->GetType();

	b2Assert(0 <= type1 && type1 < Shape::e_typeCount);
	b2Assert(0 <= type2 && type2 < Shape::e_typeCount);
	
	b2ContactCreateFcn* createFcn = s_registers[type1][type2].createFcn;
	if (createFcn)
	{
		if (s_registers[type1][type2].primary)
		{
			return createFcn(fixtureA, indexA, fixtureB, indexB);
		}
		else
		{
			return createFcn(fixtureB, indexB, fixtureA, indexA);
		}
	}
	else
	{
		return nullptr;
	}
}

void Contact::Destroy(Contact* contact)
{
	b2Assert(s_initialized == true);

	Fixture* fixtureA = contact->m_fixtureA;
	Fixture* fixtureB = contact->m_fixtureB;

	if (contact->m_manifold.pointCount > 0 &&
		fixtureA->IsSensor() == false &&
		fixtureB->IsSensor() == false)
	{
		fixtureA->GetBody()->SetAwake(true);
		fixtureB->GetBody()->SetAwake(true);
	}

	Shape::Type typeA = fixtureA->GetType();
	Shape::Type typeB = fixtureB->GetType();

	b2Assert(0 <= typeA && typeA < Shape::e_typeCount);
	b2Assert(0 <= typeB && typeB < Shape::e_typeCount);

	b2ContactDestroyFcn* destroyFcn = s_registers[typeA][typeB].destroyFcn;
	destroyFcn(contact);
}

Contact::Contact(Fixture* fA, int indexA, Fixture* fB, int indexB)
{
	m_flags = e_enabledFlag;

	m_fixtureA = fA;
	m_fixtureB = fB;

	m_indexA = indexA;
	m_indexB = indexB;

	m_manifold.pointCount = 0;

	m_nodeA.contact = nullptr;
	m_nodeA.prev = nullptr;
	m_nodeA.next = nullptr;
	m_nodeA.other = nullptr;

	m_nodeB.contact = nullptr;
	m_nodeB.prev = nullptr;
	m_nodeB.next = nullptr;
	m_nodeB.other = nullptr;

	m_toiCount = 0;

	m_friction = b2MixFriction(m_fixtureA->m_friction, m_fixtureB->m_friction);
	m_restitution = b2MixRestitution(m_fixtureA->m_restitution, m_fixtureB->m_restitution);
	m_restitutionThreshold = b2MixRestitutionThreshold(m_fixtureA->m_restitutionThreshold, m_fixtureB->m_restitutionThreshold);

	m_tangentSpeed = 0.0f;
}

// Update the contact manifold and touching status.
// Note: do not assume the fixture AABBs are overlapping or are valid.
void Contact::Update(ContactListener* listener)
{
	Manifold oldManifold = m_manifold;

	// Re-enable this contact.
	m_flags |= e_enabledFlag;

	bool touching = false;
	bool wasTouching = (m_flags & e_touchingFlag) == e_touchingFlag;

	bool sensorA = m_fixtureA->IsSensor();
	bool sensorB = m_fixtureB->IsSensor();
	bool sensor = sensorA || sensorB;

	Body* bodyA = m_fixtureA->GetBody();
	Body* bodyB = m_fixtureB->GetBody();
	const Transform& xfA = bodyA->GetTransform();
	const Transform& xfB = bodyB->GetTransform();

	// Is this contact a sensor?
	if (sensor)
	{
		const Shape* shapeA = m_fixtureA->GetShape();
		const Shape* shapeB = m_fixtureB->GetShape();
		touching = b2TestOverlap(shapeA, m_indexA, shapeB, m_indexB, xfA, xfB);

		// Sensors don't generate manifolds.
		m_manifold.pointCount = 0;
	}
	else
	{
		Evaluate(&m_manifold, xfA, xfB);
		touching = m_manifold.pointCount > 0;

		// Match old contact ids to new contact ids and copy the
		// stored impulses to warm start the solver.
		for (int i = 0; i < m_manifold.pointCount; ++i)
		{
			b2ManifoldPoint* mp2 = m_manifold.points + i;
			mp2->normalImpulse = 0.0f;
			mp2->tangentImpulse = 0.0f;
			b2ContactID id2 = mp2->id;

			for (int j = 0; j < oldManifold.pointCount; ++j)
			{
				b2ManifoldPoint* mp1 = oldManifold.points + j;

				if (mp1->id.key == id2.key)
				{
					mp2->normalImpulse = mp1->normalImpulse;
					mp2->tangentImpulse = mp1->tangentImpulse;
					break;
				}
			}
		}

		if (touching != wasTouching)
		{
			bodyA->SetAwake(true);
			bodyB->SetAwake(true);
		}
	}

	if (touching)
	{
		m_flags |= e_touchingFlag;
	}
	else
	{
		m_flags &= ~e_touchingFlag;
	}

	if (wasTouching == false && touching == true && listener)
	{
		listener->BeginContact(this);
	}

	if (wasTouching == true && touching == false && listener)
	{
		listener->EndContact(this);
	}

	if (sensor == false && touching && listener)
	{
		listener->PreSolve(this, &oldManifold);
	}
}
