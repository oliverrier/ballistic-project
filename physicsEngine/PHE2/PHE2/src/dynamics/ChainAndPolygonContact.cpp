#include "ChainAndPolygonContact.h"
#include "../collision/EdgeShape.h"
#include "../collision/ChainShape.h"

class ChainShape;

Contact* ChainAndPolygonContact::Create(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB)
{
	return new ChainAndPolygonContact(fixtureA, indexA, fixtureB, indexB);
}

void ChainAndPolygonContact::Destroy(Contact* contact)
{
	((ChainAndPolygonContact*)contact)->~ChainAndPolygonContact();
}

ChainAndPolygonContact::ChainAndPolygonContact(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB)
: Contact(fixtureA, indexA, fixtureB, indexB)
{
	b2Assert(m_fixtureA->GetType() == Shape::e_chain);
	b2Assert(m_fixtureB->GetType() == Shape::e_polygon);
}

void ChainAndPolygonContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	ChainShape* chain = (ChainShape*)m_fixtureA->GetShape();
	EdgeShape edge;
	chain->GetChildEdge(&edge, m_indexA);
	b2CollideEdgeAndPolygon(	manifold, &edge, xfA,
								(PolygonShape*)m_fixtureB->GetShape(), xfB);
}
