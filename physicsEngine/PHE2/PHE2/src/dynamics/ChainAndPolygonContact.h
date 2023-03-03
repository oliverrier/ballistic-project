#pragma once
#include "Contact.h"

class ChainAndPolygonContact : public Contact
{
public:
	static Contact* Create(	Fixture* fixtureA, int indexA,
								Fixture* fixtureB, int indexB);
	static void Destroy(Contact* contact);

	ChainAndPolygonContact(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB);
	~ChainAndPolygonContact() {}

	void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
};

