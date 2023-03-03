#pragma once
#include "Distance.h"
#include "../common/Math.h"

/// Input parameters for b2TimeOfImpact
struct TimeOfImpactInput
{
	DistanceProxy proxyA;
	DistanceProxy proxyB;
	Sweep sweepA;
	Sweep sweepB;
	float tMax;		// defines sweep interval [0, tMax]
};

/// Output parameters for b2TimeOfImpact.
struct TimeOfImpactOutput
{
	enum State
	{
		e_unknown,
		e_failed,
		e_overlapped,
		e_touching,
		e_separated
	};

	State state;
	float t;
};

/// Compute the upper bound on time before two shapes penetrate. Time is represented as
/// a fraction between [0,tMax]. This uses a swept separating axis and may miss some intermediate,
/// non-tunneling collisions. If you change the time interval, you should call this function
/// again.
/// Note: use Distance to compute the contact point and normal at the time of impact.
void b2TimeOfImpact(TimeOfImpactOutput* output, const TimeOfImpactInput* input);
