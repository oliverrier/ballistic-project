#include "PolygonContact.h"

Contact* PolygonContact::Create(Fixture* fixtureA, int, Fixture* fixtureB, int)
{
	return new PolygonContact(fixtureA, fixtureB);
}

void PolygonContact::Destroy(Contact* contact)
{
	((PolygonContact*)contact)->~PolygonContact();
}

PolygonContact::PolygonContact(Fixture* fixtureA, Fixture* fixtureB)
	: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::e_polygon);
	assert(m_fixtureB->GetType() == Shape::e_polygon);
}

void PolygonContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	CollidePolygons(	manifold,
						(PolygonShape*)m_fixtureA->GetShape(), xfA,
						(PolygonShape*)m_fixtureB->GetShape(), xfB);
}
