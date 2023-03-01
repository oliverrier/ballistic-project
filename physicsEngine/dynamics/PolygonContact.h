#pragma once
#include "Contact.h"

class PolygonContact : public Contact
{
public:
	static Contact* Create(	Fixture* fixtureA, int indexA,
								Fixture* fixtureB, int indexB);
	static void Destroy(Contact* contact);

	PolygonContact(Fixture* fixtureA, Fixture* fixtureB);
	~PolygonContact() {}

	void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
};
