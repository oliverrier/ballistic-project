#include "../collision/CircleShape.h"
#include "../collision/Collision.h"
#include "ContactSolver.h"
#include "Body.h"
#include "Contact.h"
#include "Fixture.h"

// Solver debugging is normally disabled because the block solver sometimes has to deal with a poorly conditioned effective mass matrix.
#define DEBUG_SOLVER 0

bool g_blockSolve = true;

struct ContactPositionConstraint
{
	Vec2 localPoints[maxManifoldPoints];
	Vec2 localNormal;
	Vec2 localPoint;
	int indexA;
	int indexB;
	float invMassA, invMassB;
	Vec2 localCenterA, localCenterB;
	float invIA, invIB;
	Manifold::Type type;
	float radiusA, radiusB;
	int pointCount;
};

ContactSolver::ContactSolver(ContactSolverDef* def)
{
	m_step = def->step;
	m_count = def->count;
	m_positionConstraints = std::vector<ContactPositionConstraint*>(m_count, nullptr);
	m_velocityConstraints = std::vector<ContactVelocityConstraint*>(m_count, nullptr);
	m_positions = def->positions;
	m_velocities = def->velocities;
	m_contacts = def->contacts;

	// Initialize position independent portions of the constraints.
	for (int i = 0; i < m_count; ++i)
	{
		Contact* contact = m_contacts[i];

		Fixture* fixtureA = contact->m_fixtureA;
		Fixture* fixtureB = contact->m_fixtureB;
		Shape* shapeA = fixtureA->GetShape();
		Shape* shapeB = fixtureB->GetShape();
		float radiusA = shapeA->m_radius;
		float radiusB = shapeB->m_radius;
		Body* bodyA = fixtureA->GetBody();
		Body* bodyB = fixtureB->GetBody();
		Manifold* manifold = contact->GetManifold();

		int pointCount = manifold->pointCount;
		assert(pointCount > 0);

		m_velocityConstraints[i] = new ContactVelocityConstraint;
		ContactVelocityConstraint* vc = m_velocityConstraints[i];
		vc->friction = contact->m_friction;
		vc->restitution = contact->m_restitution;
		vc->threshold = contact->m_restitutionThreshold;
		vc->tangentSpeed = contact->m_tangentSpeed;
		vc->indexA = bodyA->m_islandIndex;
		vc->indexB = bodyB->m_islandIndex;
		vc->invMassA = bodyA->m_invMass;
		vc->invMassB = bodyB->m_invMass;
		vc->invIA = bodyA->m_invI;
		vc->invIB = bodyB->m_invI;
		vc->contactIndex = i;
		vc->pointCount = pointCount;
		vc->K.SetZero();
		vc->normalMass.SetZero();

		m_positionConstraints[i] = new ContactPositionConstraint;
		ContactPositionConstraint* pc = m_positionConstraints[i];
		pc->indexA = bodyA->m_islandIndex;
		pc->indexB = bodyB->m_islandIndex;
		pc->invMassA = bodyA->m_invMass;
		pc->invMassB = bodyB->m_invMass;
		pc->localCenterA = bodyA->m_sweep.localCenter;
		pc->localCenterB = bodyB->m_sweep.localCenter;
		pc->invIA = bodyA->m_invI;
		pc->invIB = bodyB->m_invI;
		pc->localNormal = manifold->localNormal;
		pc->localPoint = manifold->localPoint;
		pc->pointCount = pointCount;
		pc->radiusA = radiusA;
		pc->radiusB = radiusB;
		pc->type = manifold->type;

		for (int j = 0; j < pointCount; ++j)
		{
			ManifoldPoint* cp = manifold->points + j;
			VelocityConstraintPoint* vcp = vc->points + j;
	
			if (m_step.warmStarting)
			{
				vcp->normalImpulse = m_step.dtRatio * cp->normalImpulse;
				vcp->tangentImpulse = m_step.dtRatio * cp->tangentImpulse;
			}
			else
			{
				vcp->normalImpulse = 0.0f;
				vcp->tangentImpulse = 0.0f;
			}

			vcp->rA.SetZero();
			vcp->rB.SetZero();
			vcp->normalMass = 0.0f;
			vcp->tangentMass = 0.0f;
			vcp->velocityBias = 0.0f;

			pc->localPoints[j] = cp->localPoint;
		}
	}
}

ContactSolver::~ContactSolver()
{
	for (auto p : m_velocityConstraints)
	{
		delete p;
	}
	m_velocityConstraints.clear();
	for (auto p : m_positionConstraints)
	{
		delete p;
	}
	m_velocityConstraints.clear();
	m_positionConstraints.clear();
}

// Initialize position dependent portions of the velocity constraints.
void ContactSolver::InitializeVelocityConstraints()
{
	for (int i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints[i];
		ContactPositionConstraint* pc = m_positionConstraints[i];

		float radiusA = pc->radiusA;
		float radiusB = pc->radiusB;
		Manifold* manifold = m_contacts[vc->contactIndex]->GetManifold();

		int indexA = vc->indexA;
		int indexB = vc->indexB;

		float mA = vc->invMassA;
		float mB = vc->invMassB;
		float iA = vc->invIA;
		float iB = vc->invIB;
		Vec2 localCenterA = pc->localCenterA;
		Vec2 localCenterB = pc->localCenterB;

		Vec2 cA = m_positions[indexA].c;
		float aA = m_positions[indexA].a;
		Vec2 vA = m_velocities[indexA].v;
		float wA = m_velocities[indexA].w;

		Vec2 cB = m_positions[indexB].c;
		float aB = m_positions[indexB].a;
		Vec2 vB = m_velocities[indexB].v;
		float wB = m_velocities[indexB].w;

		assert(manifold->pointCount > 0);

		Transform xfA, xfB;
		xfA.q.Set(aA);
		xfB.q.Set(aB);
		xfA.p = cA - Mul(xfA.q, localCenterA);
		xfB.p = cB - Mul(xfB.q, localCenterB);

		WorldManifold worldManifold;
		worldManifold.Initialize(manifold, xfA, radiusA, xfB, radiusB);

		vc->normal = worldManifold.normal;

		int pointCount = vc->pointCount;
		for (int j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			vcp->rA = worldManifold.points[j] - cA;
			vcp->rB = worldManifold.points[j] - cB;

			float rnA = Cross(vcp->rA, vc->normal);
			float rnB = Cross(vcp->rB, vc->normal);

			float kNormal = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			vcp->normalMass = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;

			Vec2 tangent = Cross(vc->normal, 1.0f);

			float rtA = Cross(vcp->rA, tangent);
			float rtB = Cross(vcp->rB, tangent);

			float kTangent = mA + mB + iA * rtA * rtA + iB * rtB * rtB;

			vcp->tangentMass = kTangent > 0.0f ? 1.0f /  kTangent : 0.0f;

			// Setup a velocity bias for restitution.
			vcp->velocityBias = 0.0f;
			float vRel = Dot(vc->normal, vB + Cross(wB, vcp->rB) - vA - Cross(wA, vcp->rA));
			if (vRel < -vc->threshold)
			{
				vcp->velocityBias = -vc->restitution * vRel;
			}
		}

		// If we have two points, then prepare the block solver->
		if (vc->pointCount == 2 && g_blockSolve)
		{
			VelocityConstraintPoint* vcp1 = vc->points + 0;
			VelocityConstraintPoint* vcp2 = vc->points + 1;

			float rn1A = Cross(vcp1->rA, vc->normal);
			float rn1B = Cross(vcp1->rB, vc->normal);
			float rn2A = Cross(vcp2->rA, vc->normal);
			float rn2B = Cross(vcp2->rB, vc->normal);

			float k11 = mA + mB + iA * rn1A * rn1A + iB * rn1B * rn1B;
			float k22 = mA + mB + iA * rn2A * rn2A + iB * rn2B * rn2B;
			float k12 = mA + mB + iA * rn1A * rn2A + iB * rn1B * rn2B;

			// Ensure a reasonable condition number.
			const float k_maxConditionNumber = 1000.0f;
			if (k11 * k11 < k_maxConditionNumber * (k11 * k22 - k12 * k12))
			{
				// K is safe to invert.
				vc->K.ex.Set(k11, k12);
				vc->K.ey.Set(k12, k22);
				vc->normalMass = vc->K.GetInverse();
			}
			else
			{
				// The constraints are redundant, just use one.
				// TODO_ERIN use deepest?
				vc->pointCount = 1;
			}
		}
	}
}

void ContactSolver::WarmStart()
{
	// Warm start.
	for (int i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints[i];

		int indexA = vc->indexA;
		int indexB = vc->indexB;
		float mA = vc->invMassA;
		float iA = vc->invIA;
		float mB = vc->invMassB;
		float iB = vc->invIB;
		int pointCount = vc->pointCount;

		Vec2 vA = m_velocities[indexA].v;
		float wA = m_velocities[indexA].w;
		Vec2 vB = m_velocities[indexB].v;
		float wB = m_velocities[indexB].w;

		Vec2 normal = vc->normal;
		Vec2 tangent = Cross(normal, 1.0f);

		for (int j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;
			Vec2 P = vcp->normalImpulse * normal + vcp->tangentImpulse * tangent;
			wA -= iA * Cross(vcp->rA, P);
			vA -= mA * P;
			wB += iB * Cross(vcp->rB, P);
			vB += mB * P;
		}

		m_velocities[indexA].v = vA;
		m_velocities[indexA].w = wA;
		m_velocities[indexB].v = vB;
		m_velocities[indexB].w = wB;
	}
}

std::vector<Velocity> ContactSolver::SolveVelocityConstraints()
{
	for (int i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints[i];

		int indexA = vc->indexA;
		int indexB = vc->indexB;
		float mA = vc->invMassA;
		float iA = vc->invIA;
		float mB = vc->invMassB;
		float iB = vc->invIB;
		int pointCount = vc->pointCount;

		Vec2 vA = m_velocities[indexA].v;
		float wA = m_velocities[indexA].w;
		Vec2 vB = m_velocities[indexB].v;
		float wB = m_velocities[indexB].w;

		Vec2 normal = vc->normal;
		Vec2 tangent = Cross(normal, 1.0f);
		float friction = vc->friction;

		assert(pointCount == 1 || pointCount == 2);

		// Solve tangent constraints first because non-penetration is more important
		// than friction.
		for (int j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			// Relative velocity at contact
			Vec2 dv = vB + Cross(wB, vcp->rB) - vA - Cross(wA, vcp->rA);

			// Compute tangent force
			float vt = Dot(dv, tangent) - vc->tangentSpeed;
			float lambda = vcp->tangentMass * (-vt);

			// Clamp the accumulated force
			float maxFriction = friction * vcp->normalImpulse;
			float newImpulse = Clamp(vcp->tangentImpulse + lambda, -maxFriction, maxFriction);
			lambda = newImpulse - vcp->tangentImpulse;
			vcp->tangentImpulse = newImpulse;

			// Apply contact impulse
			Vec2 P = lambda * tangent;

			vA -= mA * P;
			wA -= iA * Cross(vcp->rA, P);

			vB += mB * P;
			wB += iB * Cross(vcp->rB, P);
		}

		// Solve normal constraints
		if (pointCount == 1 || g_blockSolve == false)
		{
			for (int j = 0; j < pointCount; ++j)
			{
				VelocityConstraintPoint* vcp = vc->points + j;

				// Relative velocity at contact
				Vec2 dv = vB + Cross(wB, vcp->rB) - vA - Cross(wA, vcp->rA);

				// Compute normal impulse
				float vn = Dot(dv, normal);
				float lambda = -vcp->normalMass * (vn - vcp->velocityBias);

				// Clamp the accumulated impulse
				float newImpulse = Max(vcp->normalImpulse + lambda, 0.0f);
				lambda = newImpulse - vcp->normalImpulse;
				vcp->normalImpulse = newImpulse;

				// Apply contact impulse
				Vec2 P = lambda * normal;
				vA -= mA * P;
				wA -= iA * Cross(vcp->rA, P);

				vB += mB * P;
				wB += iB * Cross(vcp->rB, P);
			}
		}
		else
		{
			// Block solver developed in collaboration with Dirk Gregorius (back in 01/07 on Box2D_Lite).
			// Build the mini LCP for this contact patch
			//
			// vn = A * x + b, vn >= 0, x >= 0 and vn_i * x_i = 0 with i = 1..2
			//
			// A = J * W * JT and J = ( -n, -r1 x n, n, r2 x n )
			// b = vn0 - velocityBias
			//
			// The system is solved using the "Total enumeration method" (s. Murty). The complementary constraint vn_i * x_i
			// implies that we must have in any solution either vn_i = 0 or x_i = 0. So for the 2D contact problem the cases
			// vn1 = 0 and vn2 = 0, x1 = 0 and x2 = 0, x1 = 0 and vn2 = 0, x2 = 0 and vn1 = 0 need to be tested. The first valid
			// solution that satisfies the problem is chosen.
			// 
			// In order to account of the accumulated impulse 'a' (because of the iterative nature of the solver which only requires
			// that the accumulated impulse is clamped and not the incremental impulse) we change the impulse variable (x_i).
			//
			// Substitute:
			// 
			// x = a + d
			// 
			// a := old total impulse
			// x := new total impulse
			// d := incremental impulse 
			//
			// For the current iteration we extend the formula for the incremental impulse
			// to compute the new total impulse:
			//
			// vn = A * d + b
			//    = A * (x - a) + b
			//    = A * x + b - A * a
			//    = A * x + b'
			// b' = b - A * a;

			VelocityConstraintPoint* cp1 = vc->points + 0;
			VelocityConstraintPoint* cp2 = vc->points + 1;

			Vec2 a(cp1->normalImpulse, cp2->normalImpulse);
			assert(a.x >= 0.0f && a.y >= 0.0f);

			// Relative velocity at contact
			Vec2 dv1 = vB + Cross(wB, cp1->rB) - vA - Cross(wA, cp1->rA);
			Vec2 dv2 = vB + Cross(wB, cp2->rB) - vA - Cross(wA, cp2->rA);

			// Compute normal velocity
			float vn1 = Dot(dv1, normal);
			float vn2 = Dot(dv2, normal);

			Vec2 b;
			b.x = vn1 - cp1->velocityBias;
			b.y = vn2 - cp2->velocityBias;

			// Compute b'
			b -= Mul(vc->K, a);

			const float k_errorTol = 1e-3f;
			NOT_USED(k_errorTol);

			for (;;)
			{
				//
				// Case 1: vn = 0
				//
				// 0 = A * x + b'
				//
				// Solve for x:
				//
				// x = - inv(A) * b'
				//
				Vec2 x = - Mul(vc->normalMass, b);

				if (x.x >= 0.0f && x.y >= 0.0f)
				{
					// Get the incremental impulse
					Vec2 d = x - a;

					// Apply incremental impulse
					Vec2 P1 = d.x * normal;
					Vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (Cross(cp1->rA, P1) + Cross(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (Cross(cp1->rB, P1) + Cross(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if DEBUG_SOLVER == 1
					// Postconditions
					dv1 = vB + Cross(wB, cp1->rB) - vA - Cross(wA, cp1->rA);
					dv2 = vB + Cross(wB, cp2->rB) - vA - Cross(wA, cp2->rA);

					// Compute normal velocity
					vn1 = Dot(dv1, normal);
					vn2 = Dot(dv2, normal);

					assert(Abs(vn1 - cp1.velocityBias) < k_errorTol);
					assert(Abs(vn2 - cp2.velocityBias) < k_errorTol);
#endif
					break;
				}

				//
				// Case 2: vn1 = 0 and x2 = 0
				//
				//   0 = a11 * x1 + a12 * 0 + b1' 
				// vn2 = a21 * x1 + a22 * 0 + b2'
				//
				x.x = - cp1->normalMass * b.x;
				x.y = 0.0f;
				vn1 = 0.0f;
				vn2 = vc->K.ex.y * x.x + b.y;
				if (x.x >= 0.0f && vn2 >= 0.0f)
				{
					// Get the incremental impulse
					Vec2 d = x - a;

					// Apply incremental impulse
					Vec2 P1 = d.x * normal;
					Vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (Cross(cp1->rA, P1) + Cross(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (Cross(cp1->rB, P1) + Cross(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if DEBUG_SOLVER == 1
					// Postconditions
					dv1 = vB + Cross(wB, cp1->rB) - vA - Cross(wA, cp1->rA);

					// Compute normal velocity
					vn1 = Dot(dv1, normal);

					assert(Abs(vn1 - cp1.velocityBias) < k_errorTol);
#endif
					break;
				}


				//
				// Case 3: vn2 = 0 and x1 = 0
				//
				// vn1 = a11 * 0 + a12 * x2 + b1' 
				//   0 = a21 * 0 + a22 * x2 + b2'
				//
				x.x = 0.0f;
				x.y = - cp2->normalMass * b.y;
				vn1 = vc->K.ey.x * x.y + b.x;
				vn2 = 0.0f;

				if (x.y >= 0.0f && vn1 >= 0.0f)
				{
					// Resubstitute for the incremental impulse
					Vec2 d = x - a;

					// Apply incremental impulse
					Vec2 P1 = d.x * normal;
					Vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (Cross(cp1->rA, P1) + Cross(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (Cross(cp1->rB, P1) + Cross(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if DEBUG_SOLVER == 1
					// Postconditions
					dv2 = vB + Cross(wB, cp2->rB) - vA - Cross(wA, cp2->rA);

					// Compute normal velocity
					vn2 = Dot(dv2, normal);

					assert(Abs(vn2 - cp2.velocityBias) < k_errorTol);
#endif
					break;
				}

				//
				// Case 4: x1 = 0 and x2 = 0
				// 
				// vn1 = b1
				// vn2 = b2;
				x.x = 0.0f;
				x.y = 0.0f;
				vn1 = b.x;
				vn2 = b.y;

				if (vn1 >= 0.0f && vn2 >= 0.0f )
				{
					// Resubstitute for the incremental impulse
					Vec2 d = x - a;

					// Apply incremental impulse
					Vec2 P1 = d.x * normal;
					Vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (Cross(cp1->rA, P1) + Cross(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (Cross(cp1->rB, P1) + Cross(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

					break;
				}

				// No solution, give up. This is hit sometimes, but it doesn't seem to matter.
				break;
			}
		}

		m_velocities[indexA] = { vA, wA };
		m_velocities[indexB] = { vB, wB };
	}
	return m_velocities;
}

void ContactSolver::StoreImpulses()
{
	for (int i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints[i];
		Manifold* manifold = m_contacts[vc->contactIndex]->GetManifold();

		for (int j = 0; j < vc->pointCount; ++j)
		{
			manifold->points[j].normalImpulse = vc->points[j].normalImpulse;
			manifold->points[j].tangentImpulse = vc->points[j].tangentImpulse;
		}
	}
}

struct PositionSolverManifold
{
	void Initialize(ContactPositionConstraint* pc, const Transform& xfA, const Transform& xfB, int index)
	{
		assert(pc->pointCount > 0);

		switch (pc->type)
		{
		case Manifold::e_circles:
			{
				Vec2 pointA = Mul(xfA, pc->localPoint);
				Vec2 pointB = Mul(xfB, pc->localPoints[0]);
				normal = pointB - pointA;
				normal.Normalize();
				point = 0.5f * (pointA + pointB);
				separation = Dot(pointB - pointA, normal) - pc->radiusA - pc->radiusB;
		}
			break;

		case Manifold::e_faceA:
			{
				normal = Mul(xfA.q, pc->localNormal);
				Vec2 planePoint = Mul(xfA, pc->localPoint);

				Vec2 clipPoint = Mul(xfB, pc->localPoints[index]);
				separation = Dot(clipPoint - planePoint, normal) - pc->radiusA - pc->radiusB;
				point = clipPoint;
			}
			break;

		case Manifold::e_faceB:
			{
				normal = Mul(xfB.q, pc->localNormal);
				Vec2 planePoint = Mul(xfB, pc->localPoint);

				Vec2 clipPoint = Mul(xfA, pc->localPoints[index]);
				separation = Dot(clipPoint - planePoint, normal) - pc->radiusA - pc->radiusB;
				point = clipPoint;

				// Ensure normal points from A to B
				normal = -normal;
			}
			break;
		}
	}

	Vec2 normal;
	Vec2 point;
	float separation;
};

// Sequential solver.
bool ContactSolver::SolvePositionConstraints()
{
	float minSeparation = 0.0f;

	for (int i = 0; i < m_count; ++i)
	{
		ContactPositionConstraint* pc = m_positionConstraints[i];

		int indexA = pc->indexA;
		int indexB = pc->indexB;
		Vec2 localCenterA = pc->localCenterA;
		float mA = pc->invMassA;
		float iA = pc->invIA;
		Vec2 localCenterB = pc->localCenterB;
		float mB = pc->invMassB;
		float iB = pc->invIB;
		int pointCount = pc->pointCount;

		Vec2 cA = m_positions[indexA].c;
		float aA = m_positions[indexA].a;

		Vec2 cB = m_positions[indexB].c;
		float aB = m_positions[indexB].a;

		// Solve normal constraints
		for (int j = 0; j < pointCount; ++j)
		{
			Transform xfA, xfB;
			xfA.q.Set(aA);
			xfB.q.Set(aB);
			xfA.p = cA - Mul(xfA.q, localCenterA);
			xfB.p = cB - Mul(xfB.q, localCenterB);

			PositionSolverManifold psm;
			psm.Initialize(pc, xfA, xfB, j);
			Vec2 normal = psm.normal;

			Vec2 point = psm.point;
			float separation = psm.separation;

			Vec2 rA = point - cA;
			Vec2 rB = point - cB;

			// Track max constraint error.
			minSeparation = Min(minSeparation, separation);

			// Prevent large corrections and allow slop.
			float C = Clamp(baumgarte * (separation + linearSlop), -maxLinearCorrection, 0.0f);

			// Compute the effective mass.
			float rnA = Cross(rA, normal);
			float rnB = Cross(rB, normal);
			float K = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			// Compute normal impulse
			float impulse = K > 0.0f ? - C / K : 0.0f;

			Vec2 P = impulse * normal;

			cA -= mA * P;
			aA -= iA * Cross(rA, P);

			cB += mB * P;
			aB += iB * Cross(rB, P);
		}

		m_positions[indexA].c = cA;
		m_positions[indexA].a = aA;

		m_positions[indexB].c = cB;
		m_positions[indexB].a = aB;
	}

	// We can't expect minSpeparation >= -linearSlop because we don't
	// push the separation above -linearSlop.
	return minSeparation >= -3.0f * linearSlop;
}

// Sequential position solver for position constraints.
bool ContactSolver::SolveTOIPositionConstraints(int toiIndexA, int toiIndexB)
{
	float minSeparation = 0.0f;

	for (int i = 0; i < m_count; ++i)
	{
		ContactPositionConstraint* pc = m_positionConstraints[i];

		int indexA = pc->indexA;
		int indexB = pc->indexB;
		Vec2 localCenterA = pc->localCenterA;
		Vec2 localCenterB = pc->localCenterB;
		int pointCount = pc->pointCount;

		float mA = 0.0f;
		float iA = 0.0f;
		if (indexA == toiIndexA || indexA == toiIndexB)
		{
			mA = pc->invMassA;
			iA = pc->invIA;
		}

		float mB = 0.0f;
		float iB = 0.0f;
		if (indexB == toiIndexA || indexB == toiIndexB)
		{
			mB = pc->invMassB;
			iB = pc->invIB;
		}

		Vec2 cA = m_positions[indexA].c;
		float aA = m_positions[indexA].a;

		Vec2 cB = m_positions[indexB].c;
		float aB = m_positions[indexB].a;

		// Solve normal constraints
		for (int j = 0; j < pointCount; ++j)
		{
			Transform xfA, xfB;
			xfA.q.Set(aA);
			xfB.q.Set(aB);
			xfA.p = cA - Mul(xfA.q, localCenterA);
			xfB.p = cB - Mul(xfB.q, localCenterB);

			PositionSolverManifold psm;
			psm.Initialize(pc, xfA, xfB, j);
			Vec2 normal = psm.normal;

			Vec2 point = psm.point;
			float separation = psm.separation;

			Vec2 rA = point - cA;
			Vec2 rB = point - cB;

			// Track max constraint error.
			minSeparation = Min(minSeparation, separation);

			// Prevent large corrections and allow slop.
			float C = Clamp(toiBaumgarte * (separation + linearSlop), -maxLinearCorrection, 0.0f);

			// Compute the effective mass.
			float rnA = Cross(rA, normal);
			float rnB = Cross(rB, normal);
			float K = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			// Compute normal impulse
			float impulse = K > 0.0f ? - C / K : 0.0f;

			Vec2 P = impulse * normal;

			cA -= mA * P;
			aA -= iA * Cross(rA, P);

			cB += mB * P;
			aB += iB * Cross(rB, P);
		}

		m_positions[indexA].c = cA;
		m_positions[indexA].a = aA;

		m_positions[indexB].c = cB;
		m_positions[indexB].a = aB;
	}

	// We can't expect minSpeparation >= -linearSlop because we don't
	// push the separation above -linearSlop.
	return minSeparation >= -1.5f * linearSlop;
}
