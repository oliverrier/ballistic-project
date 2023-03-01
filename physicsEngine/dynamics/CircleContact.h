#pragma once
#include "Contact.h"

class CircleContact : public Contact
{
public:
	static Contact* Create(	Fixture* fixtureA, int indexA,
								Fixture* fixtureB, int indexB);
	static void Destroy(Contact* contact);

	CircleContact(Fixture* fixtureA, Fixture* fixtureB);
	~CircleContact() {}

	void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) override;
};

