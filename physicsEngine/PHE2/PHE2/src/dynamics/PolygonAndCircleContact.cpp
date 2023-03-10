#include "PolygonAndCircleContact.h"

Contact* PolygonAndCircleContact::Create(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB)
{
	return new PolygonAndCircleContact(fixtureA, fixtureB);
}

void PolygonAndCircleContact::Destroy(Contact* contact)
{
	((PolygonAndCircleContact*)contact)->~PolygonAndCircleContact();
}

PolygonAndCircleContact::PolygonAndCircleContact(Fixture* fixtureA, Fixture* fixtureB)
: Contact(fixtureA, 0, fixtureB, 0)
{
	b2Assert(m_fixtureA->GetType() == Shape::e_polygon);
	b2Assert(m_fixtureB->GetType() == Shape::e_circle);
}

void PolygonAndCircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	b2CollidePolygonAndCircle(	manifold,
								(PolygonShape*)m_fixtureA->GetShape(), xfA,
								(CircleShape*)m_fixtureB->GetShape(), xfB);
}
