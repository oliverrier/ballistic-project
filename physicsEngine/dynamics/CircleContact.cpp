#include "CircleContact.h"

Contact* CircleContact::Create(Fixture* fixtureA, int, Fixture* fixtureB, int)
{
	return new CircleContact(fixtureA, fixtureB);
}

void CircleContact::Destroy(Contact* contact)
{
	((CircleContact*)contact)->~CircleContact();
}

CircleContact::CircleContact(Fixture* fixtureA, Fixture* fixtureB)
	: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::e_circle);
	assert(m_fixtureB->GetType() == Shape::e_circle);
}

void CircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	CollideCircles(manifold,
					(CircleShape*)m_fixtureA->GetShape(), xfA,
					(CircleShape*)m_fixtureB->GetShape(), xfB);
}
