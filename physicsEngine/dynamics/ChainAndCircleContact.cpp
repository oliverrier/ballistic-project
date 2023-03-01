#include "ChainAndCircleContact.h"
#include "../collision/EdgeShape.h"
#include "../collision/ChainShape.h"

Contact* ChainAndCircleContact::Create(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB)
{
	return new ChainAndCircleContact(fixtureA, indexA, fixtureB, indexB);
}

void ChainAndCircleContact::Destroy(Contact* contact)
{
	((ChainAndCircleContact*)contact)->~ChainAndCircleContact();
}

ChainAndCircleContact::ChainAndCircleContact(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB)
: Contact(fixtureA, indexA, fixtureB, indexB)
{
	assert(m_fixtureA->GetType() == Shape::e_chain);
	assert(m_fixtureB->GetType() == Shape::e_circle);
}

void ChainAndCircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	ChainShape* chain = (ChainShape*)m_fixtureA->GetShape();
	EdgeShape edge;
	chain->GetChildEdge(&edge, m_indexA);
	CollideEdgeAndCircle(	manifold, &edge, xfA,
							(CircleShape*)m_fixtureB->GetShape(), xfB);
}
