#pragma once
#include "Contact.h"

class EdgeAndCircleContact : public Contact
{
public:
	static Contact* Create(	Fixture* fixtureA, int indexA,
								Fixture* fixtureB, int indexB);
	static void Destroy(Contact* contact);

	EdgeAndCircleContact(Fixture* fixtureA, Fixture* fixtureB);
	~EdgeAndCircleContact() {}

	void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
};

