#include "EdgeAndCircleContact.h"

Contact* EdgeAndCircleContact::Create(Fixture* fixtureA, int, Fixture* fixtureB, int)
{
	return new EdgeAndCircleContact(fixtureA, fixtureB);
}

void EdgeAndCircleContact::Destroy(Contact* contact)
{
	((EdgeAndCircleContact*)contact)->~EdgeAndCircleContact();
}

EdgeAndCircleContact::EdgeAndCircleContact(Fixture* fixtureA, Fixture* fixtureB)
: Contact(fixtureA, 0, fixtureB, 0)
{
	b2Assert(m_fixtureA->GetType() == Shape::e_edge);
	b2Assert(m_fixtureB->GetType() == Shape::e_circle);
}

void EdgeAndCircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	b2CollideEdgeAndCircle(	manifold,
								(EdgeShape*)m_fixtureA->GetShape(), xfA,
								(CircleShape*)m_fixtureB->GetShape(), xfB);
}
