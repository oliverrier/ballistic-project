#include "ContactManager.h"
#include "Fixture.h"
#include "Contact.h"
#include "WorldCallbacks.h"

ContactFilter b2_defaultFilter;
ContactListener b2_defaultListener;

ContactManager::ContactManager()
{
	m_contactList = {};
	m_contactCount = 0;
	m_contactFilter = &b2_defaultFilter;
	m_contactListener = &b2_defaultListener;
}

void ContactManager::Destroy(Contact* c)
{
	Fixture* fixtureA = c->GetFixtureA();
	Fixture* fixtureB = c->GetFixtureB();
	Body* bodyA = fixtureA->GetBody();
	Body* bodyB = fixtureB->GetBody();

	if (m_contactListener && c->IsTouching())
	{
		m_contactListener->EndContact(c);
	}
	
	// Call the factory.
	Contact::Destroy(c);
	--m_contactCount;
}

// This is the top level collision call for the time step. Here
// all the narrow phase collision is processed for the world
// contact list.
void ContactManager::Collide()
{
	// Update awake contacts.
	for (auto c: m_contactList)
	{
		Fixture* fixtureA = c->GetFixtureA();
		Fixture* fixtureB = c->GetFixtureB();
		int indexA = c->GetChildIndexA();
		int indexB = c->GetChildIndexB();
		Body* bodyA = fixtureA->GetBody();
		Body* bodyB = fixtureB->GetBody();
		 
		// Is this contact flagged for filtering?
		if (c->m_flags & Contact::e_filterFlag)
		{
			// Should these bodies collide?
			if (bodyB->ShouldCollide(bodyA) == false)
			{
				Contact* cNuke = c;
				c = cNuke->GetNext();
				Destroy(cNuke);
				continue;
			}

			// Check user filtering.
			if (m_contactFilter && m_contactFilter->ShouldCollide(fixtureA, fixtureB) == false)
			{
				Contact* cNuke = c;
				c = cNuke->GetNext();
				Destroy(cNuke);
				continue;
			}

			// Clear the filtering flag.
			c->m_flags &= ~Contact::e_filterFlag;
		}

		bool activeA = bodyA->IsAwake() && bodyA->m_type != b2_staticBody;
		bool activeB = bodyB->IsAwake() && bodyB->m_type != b2_staticBody;

		// At least one body must be awake and it must be dynamic or kinematic.
		if (activeA == false && activeB == false)
		{
			c = c->GetNext();
			continue;
		}

		int proxyIdA = fixtureA->m_proxies[indexA]->proxyId;
		int proxyIdB = fixtureB->m_proxies[indexB]->proxyId;
		bool overlap = m_broadPhase.TestOverlap(proxyIdA, proxyIdB);

		// Here we destroy contacts that cease to overlap in the broad-phase.
		if (overlap == false)
		{
			Contact* cNuke = c;
			c = cNuke->GetNext();
			Destroy(cNuke);
			continue;
		}

		// The contact persists.
		c->Update(m_contactListener);
	}
}

void ContactManager::FindNewContacts()
{
	m_broadPhase.UpdatePairs(this);
}

void ContactManager::AddPair(void* proxyUserDataA, void* proxyUserDataB)
{
	FixtureProxy* proxyA = (FixtureProxy*)proxyUserDataA;
	FixtureProxy* proxyB = (FixtureProxy*)proxyUserDataB;

	Fixture* fixtureA = proxyA->fixture;
	Fixture* fixtureB = proxyB->fixture;

	int indexA = proxyA->childIndex;
	int indexB = proxyB->childIndex;

	Body* bodyA = fixtureA->GetBody();
	Body* bodyB = fixtureB->GetBody();

	// Are the fixtures on the same body?
	if (bodyA == bodyB)
	{
		return;
	}

	// TODO_ERIN use a hash table to remove a potential bottleneck when both
	// bodies have a lot of contacts.
	// Does a contact already exist?
	for (auto edge: bodyB->GetContactList())
	{
		if (edge->other == bodyA)
		{
			Fixture* fA = edge->contact->GetFixtureA();
			Fixture* fB = edge->contact->GetFixtureB();
			int iA = edge->contact->GetChildIndexA();
			int iB = edge->contact->GetChildIndexB();

			if (fA == fixtureA && fB == fixtureB && iA == indexA && iB == indexB)
			{
				// A contact already exists.
				return;
			}

			if (fA == fixtureB && fB == fixtureA && iA == indexB && iB == indexA)
			{
				// A contact already exists.
				return;
			}
		}
	}

	// Does a joint override collision? Is at least one body dynamic?
	if (bodyB->ShouldCollide(bodyA) == false)
	{
		return;
	}

	// Check user filtering.
	if (m_contactFilter && m_contactFilter->ShouldCollide(fixtureA, fixtureB) == false)
	{
		return;
	}

	// Call the factory.
	Contact* c = Contact::Create(fixtureA, indexA, fixtureB, indexB);
	if (c == nullptr)
	{
		return;
	}

	// Contact creation may swap fixtures.
	fixtureA = c->GetFixtureA();
	fixtureB = c->GetFixtureB();
	bodyA = fixtureA->GetBody();
	bodyB = fixtureB->GetBody();

	// Connect to island graph.

	// Connect to body A
	c->m_nodeA.contact = c;
	c->m_nodeA.other = bodyB;

	// Connect to body B
	c->m_nodeB.contact = c;
	c->m_nodeB.other = bodyA;

	++m_contactCount;
}
