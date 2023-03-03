#include "WorldCallbacks.h"
#include "Fixture.h"

// Return true if contact calculations should be performed between these two shapes.
// If you implement your own collision filter you may want to build from this implementation.
bool ContactFilter::ShouldCollide(Fixture* fixtureA, Fixture* fixtureB)
{
	const Filter& filterA = fixtureA->GetFilterData();
	const Filter& filterB = fixtureB->GetFilterData();

	if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0)
	{
		return filterA.groupIndex > 0;
	}

	bool collide = (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;
	return collide;
}
