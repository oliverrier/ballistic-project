#pragma once

#include "Contact.h"

class EdgeAndPolygonContact : public Contact
{
public:
	static Contact* Create(	Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB);
	static void Destroy(Contact* contact);

	EdgeAndPolygonContact(Fixture* fixtureA, Fixture* fixtureB);
	~EdgeAndPolygonContact() {}

	void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
};

