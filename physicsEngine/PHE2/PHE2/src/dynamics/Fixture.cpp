#include "Fixture.h"
#include "../collision/CircleShape.h"
#include "../collision/EdgeShape.h"
#include "../collision/PolygonShape.h"
#include "../collision/ChainShape.h"
#include "Contact.h"
#include "World.h"

Fixture::Fixture()
{
	m_body = nullptr;
	m_next = nullptr;
	m_proxies = {};
	m_proxyCount = 0;
	m_shape = nullptr;
	m_density = 0.0f;
}

void Fixture::Create(Body* body, const FixtureDef* def)
{
	m_friction = def->friction;
	m_restitution = def->restitution;
	m_restitutionThreshold = def->restitutionThreshold;

	m_body = body;
	m_next = nullptr;

	m_filter = def->filter;

	m_isSensor = def->isSensor;

	m_shape = def->shape->Clone();

	// Reserve proxy space
	int childCount = m_shape->GetChildCount();
	m_proxies = std::vector<FixtureProxy*>(childCount);
	for (int i = 0; i < childCount; ++i)
	{
		m_proxies[i]->fixture = nullptr;
		m_proxies[i]->proxyId = BroadPhase::e_nullProxy;
	}
	m_proxyCount = 0;

	m_density = def->density;
}

void Fixture::Destroy()
{
	// The proxies must be destroyed before calling this.
	b2Assert(m_proxyCount == 0);

	// Free the proxy array.
	m_proxies.clear();

	// Free the child shape.
	switch (m_shape->m_type)
	{
	case Shape::e_circle:
		{
			CircleShape* s = (CircleShape*)m_shape;
			s->~CircleShape();
		}
		break;

	case Shape::e_edge:
		{
			EdgeShape* s = (EdgeShape*)m_shape;
			s->~EdgeShape();
		}
		break;

	case Shape::e_polygon:
		{
			PolygonShape* s = (PolygonShape*)m_shape;
			s->~PolygonShape();
		}
		break;

	case Shape::e_chain:
		{
			ChainShape* s = (ChainShape*)m_shape;
			s->~ChainShape();
		}
		break;

	default:
		b2Assert(false);
		break;
	}

	m_shape = nullptr;
}

void Fixture::CreateProxies(BroadPhase* broadPhase, const Transform& xf)
{
	b2Assert(m_proxyCount == 0);

	// Create proxies in the broad-phase.
	m_proxyCount = m_shape->GetChildCount();

	for (int i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies[i];
		m_shape->ComputeAABB(&proxy->aabb, xf, i);
		proxy->proxyId = broadPhase->CreateProxy(proxy->aabb, proxy);
		proxy->fixture = this;
		proxy->childIndex = i;
	}
}

void Fixture::DestroyProxies(BroadPhase* broadPhase)
{
	// Destroy proxies in the broad-phase.
	for (int i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies[i];
		broadPhase->DestroyProxy(proxy->proxyId);
		proxy->proxyId = BroadPhase::e_nullProxy;
	}

	m_proxyCount = 0;
}

void Fixture::Synchronize(BroadPhase* broadPhase, const Transform& transform1, const Transform& transform2)
{
	if (m_proxyCount == 0)
	{	
		return;
	}

	for (int i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies[i];

		// Compute an AABB that covers the swept shape (may miss some rotation effect).
		AABB aabb1, aabb2;
		m_shape->ComputeAABB(&aabb1, transform1, proxy->childIndex);
		m_shape->ComputeAABB(&aabb2, transform2, proxy->childIndex);
	
		proxy->aabb.Combine(aabb1, aabb2);

		Vec2 displacement = aabb2.GetCenter() - aabb1.GetCenter();

		broadPhase->MoveProxy(proxy->proxyId, proxy->aabb, displacement);
	}
}

void Fixture::SetFilterData(const Filter& filter)
{
	m_filter = filter;

	Refilter();
}

void Fixture::Refilter()
{
	if (m_body == nullptr)
	{
		return;
	}

	// Flag associated contacts for filtering.
	std::vector<b2ContactEdge*> edges = m_body->GetContactList();
	for (int i = 0; i < edges.size(); ++i)
	{
		auto edge = edges[i];
		Contact* contact = edge->contact;
		Fixture* fixtureA = contact->GetFixtureA();
		Fixture* fixtureB = contact->GetFixtureB();
		if (fixtureA == this || fixtureB == this)
		{
			contact->FlagForFiltering();
		}
	}

	World* world = m_body->GetWorld();

	if (world == nullptr)
	{
		return;
	}

	// Touch each proxy so that new pairs may be created
	BroadPhase* broadPhase = &world->m_contactManager.m_broadPhase;
	for (int i = 0; i < m_proxyCount; ++i)
	{
		broadPhase->TouchProxy(m_proxies[i]->proxyId);
	}
}

void Fixture::SetSensor(bool sensor)
{
	if (sensor != m_isSensor)
	{
		m_body->SetAwake(true);
		m_isSensor = sensor;
	}
}
