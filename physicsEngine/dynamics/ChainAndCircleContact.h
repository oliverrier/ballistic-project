#pragma once
#include "Contact.h"

class ChainAndCircleContact : public Contact
{
public:
	static Contact* Create(	Fixture* fixtureA, int indexA,
								Fixture* fixtureB, int indexB);
	static void Destroy(Contact* contact);

	ChainAndCircleContact(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB);
	~ChainAndCircleContact() {}

	void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
};

