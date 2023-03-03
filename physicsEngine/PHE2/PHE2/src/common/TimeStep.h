#pragma once
#include "Math.h"

/// This is an internal structure.
struct TimeStep
{
	float dt;			// time step
	float inv_dt;		// inverse time step (0 if dt == 0).
	float dtRatio;	// dt * inv_dt0
	int velocityIterations;
	int positionIterations;
	bool warmStarting;
};

/// This is an internal structure.
struct Position
{
	Vec2 c;
	float a;
};

/// This is an internal structure.
struct Velocity
{
	Vec2 v;
	float w;
};

/// Solver Data
struct SolverData
{
	TimeStep step;
	std::vector<Position*> positions;
	std::vector<Velocity*> velocities;
};

