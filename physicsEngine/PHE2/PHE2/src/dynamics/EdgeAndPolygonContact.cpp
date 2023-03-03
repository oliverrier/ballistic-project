#include "EdgeAndPolygonContact.h"
#include "../collision/PolygonShape.h"
#include "../collision/EdgeShape.h"
#include "Contact.h"

Contact* EdgeAndPolygonContact::Create(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB)
{
	return new EdgeAndPolygonContact(fixtureA, fixtureB);
}

void EdgeAndPolygonContact::Destroy(Contact* contact)
{
	((EdgeAndPolygonContact*)contact)->~EdgeAndPolygonContact();
}

EdgeAndPolygonContact::EdgeAndPolygonContact(Fixture* fixtureA, Fixture* fixtureB)
: Contact(fixtureA, 0, fixtureB, 0)
{
	b2Assert(m_fixtureA->GetType() == Shape::e_edge);
	b2Assert(m_fixtureB->GetType() == Shape::e_polygon);
}

void EdgeAndPolygonContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	b2CollideEdgeAndPolygon(	manifold,
								(EdgeShape*)m_fixtureA->GetShape(), xfA,
								(PolygonShape*)m_fixtureB->GetShape(), xfB);
}
