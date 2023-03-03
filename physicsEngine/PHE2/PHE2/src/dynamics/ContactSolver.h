#pragma once
#include <vector>
#include "../common/TimeStep.h"

struct Position;
struct Velocity;
class Contact;
class Body;
struct ContactPositionConstraint;

struct VelocityConstraintPoint
{
	Vec2 rA;
	Vec2 rB;
	float normalImpulse;
	float tangentImpulse;
	float normalMass;
	float tangentMass;
	float velocityBias;
};

struct ContactVelocityConstraint
{
	VelocityConstraintPoint points[b2_maxManifoldPoints];
	Vec2 normal;
	Mat22 normalMass;
	Mat22 K;
	int indexA;
	int indexB;
	float invMassA, invMassB;
	float invIA, invIB;
	float friction;
	float restitution;
	float threshold;
	float tangentSpeed;
	int pointCount;
	int contactIndex;
};

struct ContactSolverDef
{
	TimeStep step;
	std::vector<Contact*> contacts;
	int count;
	std::vector<Position*> positions;
	std::vector<Velocity*> velocities;
};

class ContactSolver
{
public:
	ContactSolver(ContactSolverDef* def);
	~ContactSolver();

	void InitializeVelocityConstraints();

	void WarmStart();
	void SolveVelocityConstraints();
	void StoreImpulses();

	bool SolvePositionConstraints();
	bool SolveTOIPositionConstraints(int toiIndexA, int toiIndexB);

	TimeStep m_step;
	std::vector<Position*> m_positions;
	std::vector<Velocity*> m_velocities;
	std::vector<ContactPositionConstraint*> m_positionConstraints;
	std::vector<ContactVelocityConstraint*> m_velocityConstraints;
	std::vector<Contact*> m_contacts;
	int m_count;
};

