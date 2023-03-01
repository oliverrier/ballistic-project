#include "World.h"

#include <new>

#include "Body.h"
#include "Contact.h"
#include "ContactManager.h"
#include "Island.h"
#include "../collision/BroadPhase.h"
#include "../common/Common.h"
#include "../common/Timer.h"
#include "../common/TimeStep.h"
#include "../collision/TimeOfImpact.h"
#include "../collision/Collision.h"
#include "../collision/ChainShape.h"
#include "../collision/CircleShape.h"
#include "../collision/PolygonShape.h"
#include "../collision/EdgeShape.h"

struct RayCastInput;
struct TimeStep;

World::World(const Vec2& gravity)
{
	m_destructionListener = nullptr;

	m_bodyList = nullptr;
	m_bodyCount = 0;

	m_warmStarting = true;
	m_continuousPhysics = true;
	m_subStepping = false;

	m_stepComplete = true;

	m_allowSleep = true;
	m_gravity = gravity;

	m_newContacts = false;
	m_locked = false;
	m_clearForces = true;

	m_inv_dt0 = 0.0f;

}

World::~World()
{
	Body* b = m_bodyList;
	while (b)
	{
		Body* bNext = b->m_next;

		Fixture* f = b->m_fixtureList;
		while (f)
		{
			Fixture* fNext = f->m_next;
			f->m_proxyCount = 0;
			f = fNext;
		}

		b = bNext;
	}
}

void World::SetDestructionListener(DestructionListener* listener)
{
	m_destructionListener = listener;
}

void World::SetContactFilter(ContactFilter* filter)
{
	m_contactManager.m_contactFilter = filter;
}

void World::SetContactListener(ContactListener* listener)
{
	m_contactManager.m_contactListener = listener;
}

Body* World::CreateBody(const BodyDef* def)
{
	assert(IsLocked() == false);
	if (IsLocked())
	{
		return nullptr;
	}

	// Add to world doubly linked list.
	Body* b = new Body(def, this);
	b->m_prev = nullptr;
	b->m_next = m_bodyList;
	if (m_bodyList)
	{
		m_bodyList->m_prev = b;
	}
	m_bodyList = b;
	++m_bodyCount;

	return b;
}

void World::DestroyBody(Body* b)
{
	assert(m_bodyCount > 0);
	assert(IsLocked() == false);
	if (IsLocked())
	{
		return;
	}

	// Delete the attached contacts.
	ContactEdge* ce = b->m_contactList;
	while (ce)
	{
		ContactEdge* ce0 = ce;
		ce = ce->next;
		m_contactManager.Destroy(ce0->contact);
	}
	b->m_contactList = nullptr;

	// Delete the attached fixtures. This destroys broad-phase proxies.
	Fixture* f = b->m_fixtureList;
	while (f)
	{
		Fixture* f0 = f;
		f = f->m_next;

		if (m_destructionListener)
		{
			m_destructionListener->SayGoodbye(f0);
		}

		f0->DestroyProxies(&m_contactManager.m_broadPhase);
		f0->~Fixture();

		b->m_fixtureList = f;
		b->m_fixtureCount -= 1;
	}
	b->m_fixtureList = nullptr;
	b->m_fixtureCount = 0;

	// Remove world body list.
	if (b->m_prev)
	{
		b->m_prev->m_next = b->m_next;
	}

	if (b->m_next)
	{
		b->m_next->m_prev = b->m_prev;
	}

	if (b == m_bodyList)
	{
		m_bodyList = b->m_next;
	}

	--m_bodyCount;
	b->~Body();
}

//
void World::SetAllowSleeping(bool flag)
{
	if (flag == m_allowSleep)
	{
		return;
	}

	m_allowSleep = flag;
	if (m_allowSleep == false)
	{
		for (Body* b = m_bodyList; b; b = b->m_next)
		{
			b->SetAwake(true);
		}
	}
}

// Find islands, integrate and solve constraints, solve position constraints
void World::Solve(const TimeStep& step)
{
	// Size the island for the worst case.
	Island island(m_bodyCount,
					m_contactManager.m_contactCount,
					m_contactManager.m_contactListener);

	// Clear all the island flags.
	for (Body* b = m_bodyList; b; b = b->m_next)
	{
		b->m_flags &= ~Body::e_islandFlag;
	}
	for (Contact* c = m_contactManager.m_contactList; c; c = c->m_next)
	{
		c->m_flags &= ~Contact::e_islandFlag;
	}


	// Build and simulate all awake islands.
	int stackSize = m_bodyCount;
	std::vector<Body*> stack(stackSize);
	for (Body* seed = m_bodyList; seed; seed = seed->m_next)
	{
		if (seed->m_flags & Body::e_islandFlag)
		{
			continue;
		}

		if (seed->IsAwake() == false || seed->IsEnabled() == false)
		{
			continue;
		}

		// The seed can be dynamic or kinematic.
		if (seed->GetType() == staticBody)
		{
			continue;
		}

		// Reset island and stack.
		island.Clear();
		int stackCount = 0;
		stack[stackCount++] = seed;
		seed->m_flags |= Body::e_islandFlag;

		// Perform a depth first search (DFS) on the constraint graph.
		while (stackCount > 0)
		{
			// Grab the next body off the stack and add it to the island.
			Body* b = stack[--stackCount];
			assert(b->IsEnabled() == true);
			island.Add(b);

			// To keep islands as small as possible, we don't
			// propagate islands across static bodies.
			if (b->GetType() == staticBody)
			{
				continue;
			}

			// Make sure the body is awake (without resetting sleep timer).
			b->m_flags |= Body::e_awakeFlag;

			// Search all contacts connected to this body.
			for (ContactEdge* ce = b->m_contactList; ce; ce = ce->next)
			{
				Contact* contact = ce->contact;

				// Has this contact already been added to an island?
				if (contact->m_flags & Contact::e_islandFlag)
				{
					continue;
				}

				// Is this contact solid and touching?
				if (contact->IsEnabled() == false ||
					contact->IsTouching() == false)
				{
					continue;
				}

				// Skip sensors.
				bool sensorA = contact->m_fixtureA->m_isSensor;
				bool sensorB = contact->m_fixtureB->m_isSensor;
				if (sensorA || sensorB)
				{
					continue;
				}

				island.Add(contact);
				contact->m_flags |= Contact::e_islandFlag;

				Body* other = ce->other;

				// Was the other body already added to this island?
				if (other->m_flags & Body::e_islandFlag)
				{
					continue;
				}

				assert(stackCount < stackSize);
				stack[stackCount++] = other;
				other->m_flags |= Body::e_islandFlag;
			}
		}

		island.Solve(step, m_gravity, m_allowSleep);

		// Post solve cleanup.
		for (int i = 0; i < island.m_bodyCount; ++i)
		{
			// Allow static bodies to participate in other islands.
			Body* b = island.m_bodies[i];
			if (b->GetType() == staticBody)
			{
				b->m_flags &= ~Body::e_islandFlag;
			}
		}
	}

	{
		Timer timer;
		// Synchronize fixtures, check for out of range bodies.
		for (Body* b = m_bodyList; b; b = b->GetNext())
		{
			// If a body was not in an island then it did not move.
			if ((b->m_flags & Body::e_islandFlag) == 0)
			{
				continue;
			}

			if (b->GetType() == staticBody)
			{
				continue;
			}

			// Update fixtures (for broad-phase).
			b->SynchronizeFixtures();
		}

		// Look for new contacts.
		m_contactManager.FindNewContacts();
	}
}

// Find TOI contacts and solve them.
void World::SolveTOI(const TimeStep& step)
{
	Island island(2 * maxTOIContacts, maxTOIContacts, m_contactManager.m_contactListener);

	if (m_stepComplete)
	{
		for (Body* b = m_bodyList; b; b = b->m_next)
		{
			b->m_flags &= ~Body::e_islandFlag;
			b->m_sweep.alpha0 = 0.0f;
		}

		for (Contact* c = m_contactManager.m_contactList; c; c = c->m_next)
		{
			// Invalidate TOI
			c->m_flags &= ~(Contact::e_toiFlag | Contact::e_islandFlag);
			c->m_toiCount = 0;
			c->m_toi = 1.0f;
		}
	}

	// Find TOI events and solve them.
	for (;;)
	{
		// Find the first TOI.
		Contact* minContact = nullptr;
		float minAlpha = 1.0f;

		for (Contact* c = m_contactManager.m_contactList; c; c = c->m_next)
		{
			// Is this contact disabled?
			if (c->IsEnabled() == false)
			{
				continue;
			}

			// Prevent excessive sub-stepping.
			if (c->m_toiCount > maxSubSteps)
			{
				continue;
			}

			float alpha = 1.0f;
			if (c->m_flags & Contact::e_toiFlag)
			{
				// This contact has a valid cached TOI.
				alpha = c->m_toi;
			}
			else
			{
				Fixture* fA = c->GetFixtureA();
				Fixture* fB = c->GetFixtureB();

				// Is there a sensor?
				if (fA->IsSensor() || fB->IsSensor())
				{
					continue;
				}

				Body* bA = fA->GetBody();
				Body* bB = fB->GetBody();

				BodyType typeA = bA->m_type;
				BodyType typeB = bB->m_type;
				assert(typeA == dynamicBody || typeB == dynamicBody);

				bool activeA = bA->IsAwake() && typeA != staticBody;
				bool activeB = bB->IsAwake() && typeB != staticBody;

				// Is at least one body active (awake and dynamic or kinematic)?
				if (activeA == false && activeB == false)
				{
					continue;
				}

				bool collideA = bA->IsBullet() || typeA != dynamicBody;
				bool collideB = bB->IsBullet() || typeB != dynamicBody;

				// Are these two non-bullet dynamic bodies?
				if (collideA == false && collideB == false)
				{
					continue;
				}

				// Compute the TOI for this contact.
				// Put the sweeps onto the same time interval.
				float alpha0 = bA->m_sweep.alpha0;

				if (bA->m_sweep.alpha0 < bB->m_sweep.alpha0)
				{
					alpha0 = bB->m_sweep.alpha0;
					bA->m_sweep.Advance(alpha0);
				}
				else if (bB->m_sweep.alpha0 < bA->m_sweep.alpha0)
				{
					alpha0 = bA->m_sweep.alpha0;
					bB->m_sweep.Advance(alpha0);
				}

				assert(alpha0 < 1.0f);

				int indexA = c->GetChildIndexA();
				int indexB = c->GetChildIndexB();

				// Compute the time of impact in interval [0, minTOI]
				TimeOfImpactInput input;
				input.proxyA.Set(fA->GetShape(), indexA);
				input.proxyB.Set(fB->GetShape(), indexB);
				input.sweepA = bA->m_sweep;
				input.sweepB = bB->m_sweep;
				input.tMax = 1.0f;

				TimeOfImpactOutput output;
				TimeOfImpact(&output, &input);

				// Beta is the fraction of the remaining portion of the .
				float beta = output.t;
				if (output.state == TimeOfImpactOutput::e_touching)
				{
					alpha = Min(alpha0 + (1.0f - alpha0) * beta, 1.0f);
				}
				else
				{
					alpha = 1.0f;
				}

				c->m_toi = alpha;
				c->m_flags |= Contact::e_toiFlag;
			}

			if (alpha < minAlpha)
			{
				// This is the minimum TOI found so far.
				minContact = c;
				minAlpha = alpha;
			}
		}

		if (minContact == nullptr || 1.0f - 10.0f * FLT_EPSILON < minAlpha)
		{
			// No more TOI events. Done!
			m_stepComplete = true;
			break;
		}

		// Advance the bodies to the TOI.
		Fixture* fA = minContact->GetFixtureA();
		Fixture* fB = minContact->GetFixtureB();
		Body* bA = fA->GetBody();
		Body* bB = fB->GetBody();

		Sweep backup1 = bA->m_sweep;
		Sweep backup2 = bB->m_sweep;

		bA->Advance(minAlpha);
		bB->Advance(minAlpha);

		// The TOI contact likely has some new contact points.
		minContact->Update(m_contactManager.m_contactListener);
		minContact->m_flags &= ~Contact::e_toiFlag;
		++minContact->m_toiCount;

		// Is the contact solid?
		if (minContact->IsEnabled() == false || minContact->IsTouching() == false)
		{
			// Restore the sweeps.
			minContact->SetEnabled(false);
			bA->m_sweep = backup1;
			bB->m_sweep = backup2;
			bA->SynchronizeTransform();
			bB->SynchronizeTransform();
			continue;
		}

		bA->SetAwake(true);
		bB->SetAwake(true);

		// Build the island
		island.Clear();
		island.Add(bA);
		island.Add(bB);
		island.Add(minContact);

		bA->m_flags |= Body::e_islandFlag;
		bB->m_flags |= Body::e_islandFlag;
		minContact->m_flags |= Contact::e_islandFlag;

		// Get contacts on bodyA and bodyB.
		Body* bodies[2] = {bA, bB};
		for (int i = 0; i < 2; ++i)
		{
			Body* body = bodies[i];
			if (body->m_type == dynamicBody)
			{
				for (ContactEdge* ce = body->m_contactList; ce; ce = ce->next)
				{
					if (island.m_bodyCount == island.m_bodyCapacity)
					{
						break;
					}

					if (island.m_contactCount == island.m_contactCapacity)
					{
						break;
					}

					Contact* contact = ce->contact;

					// Has this contact already been added to the island?
					if (contact->m_flags & Contact::e_islandFlag)
					{
						continue;
					}

					// Only add static, kinematic, or bullet bodies.
					Body* other = ce->other;
					if (other->m_type == dynamicBody &&
						body->IsBullet() == false && other->IsBullet() == false)
					{
						continue;
					}

					// Skip sensors.
					bool sensorA = contact->m_fixtureA->m_isSensor;
					bool sensorB = contact->m_fixtureB->m_isSensor;
					if (sensorA || sensorB)
					{
						continue;
					}

					// Tentatively advance the body to the TOI.
					Sweep backup = other->m_sweep;
					if ((other->m_flags & Body::e_islandFlag) == 0)
					{
						other->Advance(minAlpha);
					}

					// Update the contact points
					contact->Update(m_contactManager.m_contactListener);

					// Was the contact disabled by the user?
					if (contact->IsEnabled() == false)
					{
						other->m_sweep = backup;
						other->SynchronizeTransform();
						continue;
					}

					// Are there contact points?
					if (contact->IsTouching() == false)
					{
						other->m_sweep = backup;
						other->SynchronizeTransform();
						continue;
					}

					// Add the contact to the island
					contact->m_flags |= Contact::e_islandFlag;
					island.Add(contact);

					// Has the other body already been added to the island?
					if (other->m_flags & Body::e_islandFlag)
					{
						continue;
					}
					
					// Add the other body to the island.
					other->m_flags |= Body::e_islandFlag;

					if (other->m_type != staticBody)
					{
						other->SetAwake(true);
					}

					island.Add(other);
				}
			}
		}

		TimeStep subStep;
		subStep.dt = (1.0f - minAlpha) * step.dt;
		subStep.inv_dt = 1.0f / subStep.dt;
		subStep.dtRatio = 1.0f;
		subStep.positionIterations = 20;
		subStep.velocityIterations = step.velocityIterations;
		subStep.warmStarting = false;
		island.SolveTOI(subStep, bA->m_islandIndex, bB->m_islandIndex);

		// Reset island flags and synchronize broad-phase proxies.
		for (int i = 0; i < island.m_bodyCount; ++i)
		{
			Body* body = island.m_bodies[i];
			body->m_flags &= ~Body::e_islandFlag;

			if (body->m_type != dynamicBody)
			{
				continue;
			}

			body->SynchronizeFixtures();

			// Invalidate all contact TOIs on this displaced body.
			for (ContactEdge* ce = body->m_contactList; ce; ce = ce->next)
			{
				ce->contact->m_flags &= ~(Contact::e_toiFlag | Contact::e_islandFlag);
			}
		}

		// Commit fixture proxy movements to the broad-phase so that new contacts are created.
		// Also, some contacts can be destroyed.
		m_contactManager.FindNewContacts();

		if (m_subStepping)
		{
			m_stepComplete = false;
			break;
		}
	}
}

void World::Step(float dt, int velocityIterations, int positionIterations)
{
	Timer stepTimer;

	// If new fixtures were added, we need to find the new contacts.
	if (m_newContacts)
	{
		m_contactManager.FindNewContacts();
		m_newContacts = false;
	}

	m_locked = true;

	TimeStep step;
	step.dt = dt;
	step.velocityIterations	= velocityIterations;
	step.positionIterations = positionIterations;
	if (dt > 0.0f)
	{
		step.inv_dt = 1.0f / dt;
	}
	else
	{
		step.inv_dt = 0.0f;
	}

	step.dtRatio = m_inv_dt0 * dt;

	step.warmStarting = m_warmStarting;
	
	// Update contacts. This is where some contacts are destroyed.
	{
		Timer timer;
		m_contactManager.Collide();
	}

	// Integrate velocities, solve velocity constraints, and integrate positions.
	if (m_stepComplete && step.dt > 0.0f)
	{
		Timer timer;
		Solve(step);
	}

	// Handle TOI events.
	if (m_continuousPhysics && step.dt > 0.0f)
	{
		Timer timer;
		SolveTOI(step);
	}

	if (step.dt > 0.0f)
	{
		m_inv_dt0 = step.inv_dt;
	}

	if (m_clearForces)
	{
		ClearForces();
	}

	m_locked = false;
}

void World::ClearForces()
{
	for (Body* body = m_bodyList; body; body = body->GetNext())
	{
		body->m_force.SetZero();
		body->m_torque = 0.0f;
	}
}

struct WorldRayCastWrapper
{
	float rayCastCallback(const RayCastInput& input, int proxyId)
	{
		void* userData = broadPhase->GetUserData(proxyId);
		FixtureProxy* proxy = (FixtureProxy*)userData;
		Fixture* fixture = proxy->fixture;
		int index = proxy->childIndex;
		RayCastOutput output;
		bool hit = fixture->RayCast(&output, input, index);

		if (hit)
		{
			float fraction = output.fraction;
			Vec2 point = (1.0f - fraction) * input.p1 + fraction * input.p2;
			return callback->ReportFixture(fixture, point, output.normal, fraction);
		}

		return input.maxFraction;
	}

	const BroadPhase* broadPhase;
	RayCastCallback* callback;
};

void World::RayCast(RayCastCallback* callback, const Vec2& point1, const Vec2& point2) const
{
	WorldRayCastWrapper wrapper;
	wrapper.broadPhase = &m_contactManager.m_broadPhase;
	wrapper.callback = callback;
	RayCastInput input;
	input.maxFraction = 1.0f;
	input.p1 = point1;
	input.p2 = point2;
	m_contactManager.m_broadPhase.RayCast(&wrapper, input);
}

int World::GetProxyCount() const
{
	return m_contactManager.m_broadPhase.GetProxyCount();
}

int World::GetTreeHeight() const
{
	return m_contactManager.m_broadPhase.GetTreeHeight();
}

int World::GetTreeBalance() const
{
	return m_contactManager.m_broadPhase.GetTreeBalance();
}

float World::GetTreeQuality() const
{
	return m_contactManager.m_broadPhase.GetTreeQuality();
}

void World::ShiftOrigin(const Vec2& newOrigin)
{
	assert(m_locked == false);
	if (m_locked)
	{
		return;
	}

	for (Body* b = m_bodyList; b; b = b->m_next)
	{
		b->m_xf.p -= newOrigin;
		b->m_sweep.c0 -= newOrigin;
		b->m_sweep.c -= newOrigin;
	}

	m_contactManager.m_broadPhase.ShiftOrigin(newOrigin);
}

