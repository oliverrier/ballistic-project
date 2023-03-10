#include "TimeOfImpact.h"
#include "../common/Common.h"
#include "../common/Timer.h"

float b2_toiTime, b2_toiMaxTime;
int b2_toiCalls, b2_toiIters, b2_toiMaxIters;
int b2_toiRootIters, b2_toiMaxRootIters;

struct b2SeparationFunction
{
	enum Type
	{
		e_points,
		e_faceA,
		e_faceB
	};

	// TODO_ERIN might not need to return the separation

	float Initialize(const SimplexCache* cache,
		const DistanceProxy* proxyA, const Sweep& sweepA,
		const DistanceProxy* proxyB, const Sweep& sweepB,
		float t1)
	{
		m_proxyA = proxyA;
		m_proxyB = proxyB;
		int count = cache->count;
		b2Assert(0 < count && count < 3);

		m_sweepA = sweepA;
		m_sweepB = sweepB;

		Transform xfA, xfB;
		m_sweepA.GetTransform(&xfA, t1);
		m_sweepB.GetTransform(&xfB, t1);

		if (count == 1)
		{
			m_type = e_points;
			Vec2 localPointA = m_proxyA->GetVertex(cache->indexA[0]);
			Vec2 localPointB = m_proxyB->GetVertex(cache->indexB[0]);
			Vec2 pointA = Mul(xfA, localPointA);
			Vec2 pointB = Mul(xfB, localPointB);
			m_axis = pointB - pointA;
			float s = m_axis.Normalize();
			return s;
		}
		if (cache->indexA[0] == cache->indexA[1])
		{
			// Two points on B and one on A.
			m_type = e_faceB;
			Vec2 localPointB1 = proxyB->GetVertex(cache->indexB[0]);
			Vec2 localPointB2 = proxyB->GetVertex(cache->indexB[1]);

			m_axis = Cross(localPointB2 - localPointB1, 1.0f);
			m_axis.Normalize();
			Vec2 normal = Mul(xfB.q, m_axis);

			m_localPoint = 0.5f * (localPointB1 + localPointB2);
			Vec2 pointB = Mul(xfB, m_localPoint);

			Vec2 localPointA = proxyA->GetVertex(cache->indexA[0]);
			Vec2 pointA = Mul(xfA, localPointA);

			float s = Dot(pointA - pointB, normal);
			if (s < 0.0f)
			{
				m_axis = -m_axis;
				s = -s;
			}
			return s;
		}
		else
		{
			// Two points on A and one or two points on B.
			m_type = e_faceA;
			Vec2 localPointA1 = m_proxyA->GetVertex(cache->indexA[0]);
			Vec2 localPointA2 = m_proxyA->GetVertex(cache->indexA[1]);
			
			m_axis = Cross(localPointA2 - localPointA1, 1.0f);
			m_axis.Normalize();
			Vec2 normal = Mul(xfA.q, m_axis);

			m_localPoint = 0.5f * (localPointA1 + localPointA2);
			Vec2 pointA = Mul(xfA, m_localPoint);

			Vec2 localPointB = m_proxyB->GetVertex(cache->indexB[0]);
			Vec2 pointB = Mul(xfB, localPointB);

			float s = Dot(pointB - pointA, normal);
			if (s < 0.0f)
			{
				m_axis = -m_axis;
				s = -s;
			}
			return s;
		}
	}

	//
	float FindMinSeparation(int* indexA, int* indexB, float t) const
	{
		Transform xfA, xfB;
		m_sweepA.GetTransform(&xfA, t);
		m_sweepB.GetTransform(&xfB, t);

		switch (m_type)
		{
		case e_points:
			{
				Vec2 axisA = MulT(xfA.q,  m_axis);
				Vec2 axisB = MulT(xfB.q, -m_axis);

				*indexA = m_proxyA->GetSupport(axisA);
				*indexB = m_proxyB->GetSupport(axisB);

				Vec2 localPointA = m_proxyA->GetVertex(*indexA);
				Vec2 localPointB = m_proxyB->GetVertex(*indexB);
				
				Vec2 pointA = Mul(xfA, localPointA);
				Vec2 pointB = Mul(xfB, localPointB);

				float separation = Dot(pointB - pointA, m_axis);
				return separation;
			}

		case e_faceA:
			{
				Vec2 normal = Mul(xfA.q, m_axis);
				Vec2 pointA = Mul(xfA, m_localPoint);

				Vec2 axisB = MulT(xfB.q, -normal);
				
				*indexA = -1;
				*indexB = m_proxyB->GetSupport(axisB);

				Vec2 localPointB = m_proxyB->GetVertex(*indexB);
				Vec2 pointB = Mul(xfB, localPointB);

				float separation = Dot(pointB - pointA, normal);
				return separation;
			}

		case e_faceB:
			{
				Vec2 normal = Mul(xfB.q, m_axis);
				Vec2 pointB = Mul(xfB, m_localPoint);

				Vec2 axisA = MulT(xfA.q, -normal);

				*indexB = -1;
				*indexA = m_proxyA->GetSupport(axisA);

				Vec2 localPointA = m_proxyA->GetVertex(*indexA);
				Vec2 pointA = Mul(xfA, localPointA);

				float separation = Dot(pointA - pointB, normal);
				return separation;
			}

		default:
			b2Assert(false);
			*indexA = -1;
			*indexB = -1;
			return 0.0f;
		}
	}

	//
	float Evaluate(int indexA, int indexB, float t) const
	{
		Transform xfA, xfB;
		m_sweepA.GetTransform(&xfA, t);
		m_sweepB.GetTransform(&xfB, t);

		switch (m_type)
		{
		case e_points:
			{
				Vec2 localPointA = m_proxyA->GetVertex(indexA);
				Vec2 localPointB = m_proxyB->GetVertex(indexB);

				Vec2 pointA = Mul(xfA, localPointA);
				Vec2 pointB = Mul(xfB, localPointB);
				float separation = Dot(pointB - pointA, m_axis);

				return separation;
			}

		case e_faceA:
			{
				Vec2 normal = Mul(xfA.q, m_axis);
				Vec2 pointA = Mul(xfA, m_localPoint);

				Vec2 localPointB = m_proxyB->GetVertex(indexB);
				Vec2 pointB = Mul(xfB, localPointB);

				float separation = Dot(pointB - pointA, normal);
				return separation;
			}

		case e_faceB:
			{
				Vec2 normal = Mul(xfB.q, m_axis);
				Vec2 pointB = Mul(xfB, m_localPoint);

				Vec2 localPointA = m_proxyA->GetVertex(indexA);
				Vec2 pointA = Mul(xfA, localPointA);

				float separation = Dot(pointA - pointB, normal);
				return separation;
			}

		default:
			b2Assert(false);
			return 0.0f;
		}
	}

	const DistanceProxy* m_proxyA;
	const DistanceProxy* m_proxyB;
	Sweep m_sweepA, m_sweepB;
	Type m_type;
	Vec2 m_localPoint;
	Vec2 m_axis;
};

// CCD via the local separating axis method. This seeks progression
// by computing the largest time at which separation is maintained.
void b2TimeOfImpact(TimeOfImpactOutput* output, const TimeOfImpactInput* input)
{
	Timer timer;

	++b2_toiCalls;

	output->state = TimeOfImpactOutput::e_unknown;
	output->t = input->tMax;

	const DistanceProxy* proxyA = &input->proxyA;
	const DistanceProxy* proxyB = &input->proxyB;

	Sweep sweepA = input->sweepA;
	Sweep sweepB = input->sweepB;

	// Large rotations can make the root finder fail, so we normalize the
	// sweep angles.
	sweepA.Normalize();
	sweepB.Normalize();

	float tMax = input->tMax;

	float totalRadius = proxyA->m_radius + proxyB->m_radius;
	float target = Max(b2_linearSlop, totalRadius - 3.0f * b2_linearSlop);
	float tolerance = 0.25f * b2_linearSlop;
	b2Assert(target > tolerance);

	float t1 = 0.0f;
	const int k_maxIterations = 20;	// TODO_ERIN b2Settings
	int iter = 0;

	// Prepare input for distance query.
	SimplexCache cache;
	cache.count = 0;
	DistanceInput distanceInput;
	distanceInput.proxyA = input->proxyA;
	distanceInput.proxyB = input->proxyB;
	distanceInput.useRadii = false;

	// The outer loop progressively attempts to compute new separating axes.
	// This loop terminates when an axis is repeated (no progress is made).
	for(;;)
	{
		Transform xfA, xfB;
		sweepA.GetTransform(&xfA, t1);
		sweepB.GetTransform(&xfB, t1);

		// Get the distance between shapes. We can also use the results
		// to get a separating axis.
		distanceInput.transformA = xfA;
		distanceInput.transformB = xfB;
		DistanceOutput distanceOutput;
		Distance(&distanceOutput, &cache, &distanceInput);

		// If the shapes are overlapped, we give up on continuous collision.
		if (distanceOutput.distance <= 0.0f)
		{
			// Failure!
			output->state = TimeOfImpactOutput::e_overlapped;
			output->t = 0.0f;
			break;
		}

		if (distanceOutput.distance < target + tolerance)
		{
			// Victory!
			output->state = TimeOfImpactOutput::e_touching;
			output->t = t1;
			break;
		}

		// Initialize the separating axis.
		b2SeparationFunction fcn;
		fcn.Initialize(&cache, proxyA, sweepA, proxyB, sweepB, t1);
#if 0
		// Dump the curve seen by the root finder
		{
			const int N = 100;
			float dx = 1.0f / N;
			float xs[N+1];
			float fs[N+1];

			float x = 0.0f;

			for (int i = 0; i <= N; ++i)
			{
				sweepA.GetTransform(&xfA, x);
				sweepB.GetTransform(&xfB, x);
				float f = fcn.Evaluate(xfA, xfB) - target;

				printf("%g %g\n", x, f);

				xs[i] = x;
				fs[i] = f;

				x += dx;
			}
		}
#endif

		// Compute the TOI on the separating axis. We do this by successively
		// resolving the deepest point. This loop is bounded by the number of vertices.
		bool done = false;
		float t2 = tMax;
		int pushBackIter = 0;
		for (;;)
		{
			// Find the deepest point at t2. Store the witness point indices.
			int indexA, indexB;
			float s2 = fcn.FindMinSeparation(&indexA, &indexB, t2);

			// Is the final configuration separated?
			if (s2 > target + tolerance)
			{
				// Victory!
				output->state = TimeOfImpactOutput::e_separated;
				output->t = tMax;
				done = true;
				break;
			}

			// Has the separation reached tolerance?
			if (s2 > target - tolerance)
			{
				// Advance the sweeps
				t1 = t2;
				break;
			}

			// Compute the initial separation of the witness points.
			float s1 = fcn.Evaluate(indexA, indexB, t1);

			// Check for initial overlap. This might happen if the root finder
			// runs out of iterations.
			if (s1 < target - tolerance)
			{
				output->state = TimeOfImpactOutput::e_failed;
				output->t = t1;
				done = true;
				break;
			}

			// Check for touching
			if (s1 <= target + tolerance)
			{
				// Victory! t1 should hold the TOI (could be 0.0).
				output->state = TimeOfImpactOutput::e_touching;
				output->t = t1;
				done = true;
				break;
			}

			// Compute 1D root of: f(x) - target = 0
			int rootIterCount = 0;
			float a1 = t1, a2 = t2;
			for (;;)
			{
				// Use a mix of the secant rule and bisection.
				float t;
				if (rootIterCount & 1)
				{
					// Secant rule to improve convergence.
					t = a1 + (target - s1) * (a2 - a1) / (s2 - s1);
				}
				else
				{
					// Bisection to guarantee progress.
					t = 0.5f * (a1 + a2);
				}

				++rootIterCount;
				++b2_toiRootIters;

				float s = fcn.Evaluate(indexA, indexB, t);

				if (Abs(s - target) < tolerance)
				{
					// t2 holds a tentative value for t1
					t2 = t;
					break;
				}

				// Ensure we continue to bracket the root.
				if (s > target)
				{
					a1 = t;
					s1 = s;
				}
				else
				{
					a2 = t;
					s2 = s;
				}
				
				if (rootIterCount == 50)
				{
					break;
				}
			}

			b2_toiMaxRootIters = Max(b2_toiMaxRootIters, rootIterCount);

			++pushBackIter;

			if (pushBackIter == b2_maxPolygonVertices)
			{
				break;
			}
		}

		++iter;
		++b2_toiIters;

		if (done)
		{
			break;
		}

		if (iter == k_maxIterations)
		{
			// Root finder got stuck. Semi-victory.
			output->state = TimeOfImpactOutput::e_failed;
			output->t = t1;
			break;
		}
	}

	b2_toiMaxIters = Max(b2_toiMaxIters, iter);

	float time = timer.GetMilliseconds();
	b2_toiMaxTime = Max(b2_toiMaxTime, time);
	b2_toiTime += time;
}
