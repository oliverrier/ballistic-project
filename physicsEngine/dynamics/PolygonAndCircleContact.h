#pragma once
#include "Contact.h"

class PolygonAndCircleContact : public Contact
{
public:
	static Contact* Create(Fixture* fixtureA, int indexA, Fixture* fixtureB, int indexB);
	static void Destroy(Contact* contact);

	PolygonAndCircleContact(Fixture* fixtureA, Fixture* fixtureB);
	~PolygonAndCircleContact() {}

	void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
};

