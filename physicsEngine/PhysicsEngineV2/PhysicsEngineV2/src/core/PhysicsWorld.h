#pragma once
#include <algorithm>
#include <array>
#include <memory>

#include "Collision.h"
#include "Manifold.h"
#include "RigidBody.h"
#include "../utils/Math/Vector2.h"

class PhysicsWorld
{
	inline static int MinIterations = 1;
	inline static int MaxIterations = 128;

    inline static FVector2 gravity{ 0.f, 9.81f };
    std::vector<std::shared_ptr<RigidBody>> rigidBodies;
    std::vector<std::pair<int, int>> contactPairs;

    std::array<FVector2, 2> contactList;
    std::array<FVector2, 2> impulseList;
    std::array<FVector2, 2> raList;
    std::array<FVector2, 2> rbList;
    std::array<FVector2, 2> frictionImpulseList;
    std::array<float, 2> jList;


private:
    PhysicsWorld() = default;

public:

    static std::shared_ptr<PhysicsWorld> GetWorld()
    {
        static std::shared_ptr<PhysicsWorld> instance{ new PhysicsWorld };
        return instance;
    }

    void AddRigidBody(const std::shared_ptr<RigidBody>& rigid_body)
    {
        rigidBodies.push_back(rigid_body);
    }

    void RemoveRigidBody(const std::shared_ptr<RigidBody>& rigid_body)
    {
        const auto it = std::ranges::find(rigidBodies, rigid_body);
        if (it != rigidBodies.end()) {
            rigidBodies.erase(it);
        }
    }


    bool GetBody(int index, std::shared_ptr<RigidBody>& rigid_body)
    {
        rigid_body = nullptr;

        if (index < 0 || index >= rigidBodies.size())
        {
            return false;
        }

        rigid_body = rigidBodies[index];
        return true;
    }

    void Step(float time, int totalIterations)
    {
        totalIterations = std::clamp(totalIterations, MinIterations, MaxIterations);

        for (int currentIteration = 0; currentIteration < totalIterations; ++currentIteration)
        {
            contactPairs.clear();
            StepBodies(time, totalIterations);
            BroadPhase();
            NarrowPhase();
        }
    }

    void BroadPhase() {
        // collision step
        for (int i = 0; i < rigidBodies.size() - 1; i++)
        {
            auto& bodyA = rigidBodies[i];
            auto bodyA_aabb = bodyA->GetAABB();

            for (int j = i + 1; j < rigidBodies.size(); j++)
            {
                auto& bodyB = rigidBodies[j];
                auto bodyB_aabb = bodyB->GetAABB();

                if (bodyA->IsStatic && bodyB->IsStatic)
                {
                    continue;
                }

                if(!Collisions::IntersectAABBs(bodyA_aabb, bodyB_aabb))
                {
                    continue;
                }

                contactPairs.push_back(std::pair(i, j));
            }
        }
    }

    void NarrowPhase()
    {
        for (int i = 0; i < contactPairs.size(); i++)
        {
            std::pair<int, int> pair = contactPairs[i];
            auto bodyA = rigidBodies[pair.first];
            auto bodyB = rigidBodies[pair.second];

            FVector2 normal;
            float depth;
            if (Collisions::Collide(bodyA, bodyB, normal, depth))
            {
                SeparateBodies(bodyA, bodyB, normal * depth);
                FVector2 contact1;
                FVector2 contact2;
                int contactCount;
                Collisions::FindContactPoints(bodyA, bodyB, contact1, contact2, contactCount);
                Manifold contact = Manifold(bodyA, bodyB, normal, depth, contact1, contact2, contactCount);
                ResolveCollisionWithRotationAndFriction(contact);
            }
        }
    }

    void StepBodies(float time, int totalIterations) {
        for (int i = 0; i < rigidBodies.size(); i++)
        {
            rigidBodies[i]->Step(time, gravity, totalIterations);
        }
    }

    void SeparateBodies(std::shared_ptr<RigidBody>& bodyA, std::shared_ptr<RigidBody>& bodyB, FVector2 mtv)
    {
        if (bodyA->IsStatic)
        {
            bodyB->Move(mtv);
        }
        else if (bodyB->IsStatic)
        {
            bodyA->Move(-mtv);
        }
        else
        {
            bodyA->Move(-mtv / 2.f);
            bodyB->Move(mtv / 2.f);
        }
    }


    void ResolveCollisionBasic(Manifold& contact)
    {
        std::shared_ptr<RigidBody>& bodyA = contact.BodyA;
        std::shared_ptr<RigidBody>& bodyB = contact.BodyB;
        FVector2 normal = contact.Normal;
        float depth = contact.Depth;

        FVector2 relativeVelocity = bodyB->linearVelocity - bodyA->linearVelocity;

        if (relativeVelocity.Dot(normal) > 0.f)
        {
            return;
        }

        float e = std::min(bodyA->Restitution, bodyB->Restitution);

        float j = -(1.f + e) * relativeVelocity.Dot(normal);
        j /= bodyA->InvMass + bodyB->InvMass;

        FVector2 impulse = normal * j;

        bodyA->linearVelocity -= impulse * bodyA->InvMass;
        bodyB->linearVelocity += impulse * bodyB->InvMass;
    }

    void ResolveCollisionWithRotation(Manifold& contact)
    {
        std::shared_ptr<RigidBody>& bodyA = contact.BodyA;
        std::shared_ptr<RigidBody>& bodyB = contact.BodyB;
        FVector2 normal = contact.Normal;
        FVector2 contact1 = contact.Contact1;
        FVector2 contact2 = contact.Contact2;
        int contactCount = contact.ContactCount;

        float e = std::min(bodyA->Restitution, bodyB->Restitution);

        contactList[0] = contact1;
        contactList[1] = contact2;

        for (int i = 0; i < contactCount; i++)
        {
            impulseList[i] = FVector2::Zero();
            raList[i] = FVector2::Zero();
            rbList[i] = FVector2::Zero();
        }

        for (int i = 0; i < contactCount; i++)
        {
            FVector2 ra = contactList[i] - bodyA->position;
            FVector2 rb = contactList[i] - bodyB->position;

            raList[i] = ra;
            rbList[i] = rb;

            FVector2 raPerp = FVector2(-ra.Y, ra.X);
            FVector2 rbPerp = FVector2(-rb.Y, rb.X);

            FVector2 angularLinearVelocityA = raPerp * bodyA->angularVelocity;
            FVector2 angularLinearVelocityB = rbPerp * bodyB->angularVelocity;

            FVector2 relativeVelocity =
                (bodyB->linearVelocity + angularLinearVelocityB) -
                (bodyA->linearVelocity + angularLinearVelocityA);

            float contactVelocityMag = relativeVelocity.Dot(normal);

            if (contactVelocityMag > 0.f)
            {
                continue;
            }

            float raPerpDotN = raPerp.Dot(normal);
            float rbPerpDotN = rbPerp.Dot(normal);

            float denom = bodyA->InvMass + bodyB->InvMass +
                (raPerpDotN * raPerpDotN) * bodyA->InvInertia +
                (rbPerpDotN * rbPerpDotN) * bodyB->InvInertia;

            float j = -(1.f + e) * contactVelocityMag;
            j /= denom;
            j /= (float)contactCount;

            FVector2 impulse = normal * j;
            impulseList[i] = impulse;
        }

        for (int i = 0; i < contactCount; i++)
        {
            FVector2 impulse = impulseList[i];
            FVector2 ra = raList[i];
            FVector2 rb = rbList[i];

            bodyA->linearVelocity += -impulse * bodyA->InvMass;
            bodyA->angularVelocity += -FVector2::CrossProduct(ra, impulse) * bodyA->InvInertia;
            bodyB->linearVelocity += impulse * bodyB->InvMass;
            bodyB->angularVelocity += FVector2::CrossProduct(rb, impulse) * bodyB->InvInertia;
        }
    }


    void ResolveCollisionWithRotationAndFriction(Manifold& contact)
    {
        std::shared_ptr<RigidBody>& bodyA = contact.BodyA;
        std::shared_ptr<RigidBody>& bodyB = contact.BodyB;
        FVector2 normal = contact.Normal;
        FVector2 contact1 = contact.Contact1;
        FVector2 contact2 = contact.Contact2;
        int contactCount = contact.ContactCount;

        float e = std::min(bodyA->Restitution, bodyB->Restitution);

        float sf = (bodyA->StaticFriction + bodyB->StaticFriction) * 0.5f;
        float df = (bodyA->DynamicFriction + bodyB->DynamicFriction) * 0.5f;

        contactList[0] = contact1;
        contactList[1] = contact2;

        for (int i = 0; i < contactCount; i++)
        {
            impulseList[i] = FVector2::Zero();
            raList[i] = FVector2::Zero();
            rbList[i] = FVector2::Zero();
            frictionImpulseList[i] = FVector2::Zero();
            jList[i] = 0.f;
        }

        for (int i = 0; i < contactCount; i++)
        {
            FVector2 ra = contactList[i] - bodyA->position;
            FVector2 rb = contactList[i] - bodyB->position;

            raList[i] = ra;
            rbList[i] = rb;

            FVector2 raPerp = FVector2(-ra.Y, ra.X);
            FVector2 rbPerp = FVector2(-rb.Y, rb.X);

            FVector2 angularLinearVelocityA = raPerp * bodyA->angularVelocity;
            FVector2 angularLinearVelocityB = rbPerp * bodyB->angularVelocity;

            FVector2 relativeVelocity =
                (bodyB->linearVelocity + angularLinearVelocityB) -
                (bodyA->linearVelocity + angularLinearVelocityA);

            float contactVelocityMag = relativeVelocity.Dot(normal);

            if (contactVelocityMag > 0.f)
            {
                continue;
            }

            float raPerpDotN = raPerp.Dot(normal);
            float rbPerpDotN = rbPerp.Dot(normal);

            float denom = bodyA->InvMass + bodyB->InvMass +
                (raPerpDotN * raPerpDotN) * bodyA->InvInertia +
                (rbPerpDotN * rbPerpDotN) * bodyB->InvInertia;

            float j = -(1.f + e) * contactVelocityMag;
            j /= denom;
            j /= (float)contactCount;

            jList[i] = j;

            FVector2 impulse = normal * j;
            impulseList[i] = impulse;
        }

        for (int i = 0; i < contactCount; i++)
        {
            FVector2 impulse = impulseList[i];
            FVector2 ra = raList[i];
            FVector2 rb = rbList[i];

            bodyA->linearVelocity += -impulse * bodyA->InvMass;
            bodyA->angularVelocity += -FVector2::CrossProduct(ra, impulse) * bodyA->InvInertia;
            bodyB->linearVelocity += impulse * bodyB->InvMass;
            bodyB->angularVelocity += FVector2::CrossProduct(rb, impulse) * bodyB->InvInertia;
        }

        for (int i = 0; i < contactCount; i++)
        {
            FVector2 ra = contactList[i] - bodyA->position;
            FVector2 rb = contactList[i] - bodyB->position;

            raList[i] = ra;
            rbList[i] = rb;

            FVector2 raPerp = FVector2(-ra.Y, ra.X);
            FVector2 rbPerp = FVector2(-rb.Y, rb.X);

            FVector2 angularLinearVelocityA = raPerp * bodyA->angularVelocity;
            FVector2 angularLinearVelocityB = rbPerp * bodyB->angularVelocity;

            FVector2 relativeVelocity =
                (bodyB->linearVelocity + angularLinearVelocityB) -
                (bodyA->linearVelocity + angularLinearVelocityA);

            FVector2 tangent = relativeVelocity - normal * relativeVelocity.Dot(normal);

            if (NearlyEqual(tangent, FVector2::Zero()))
            {
                continue;
            }
            else
            {
                tangent = tangent.Normalized();
            }

            float raPerpDotT = raPerp.Dot(tangent);
            float rbPerpDotT = rbPerp.Dot(tangent);

            float denom = bodyA->InvMass + bodyB->InvMass +
                (raPerpDotT * raPerpDotT) * bodyA->InvInertia +
                (rbPerpDotT * rbPerpDotT) * bodyB->InvInertia;

            float jt = -relativeVelocity.Dot(tangent);
            jt /= denom;
            jt /= (float)contactCount;

            FVector2 frictionImpulse;
            float j = jList[i];

            if (std::abs(jt) <= j * sf)
            {
                frictionImpulse = tangent * jt;
            }
            else
            {
                frictionImpulse = tangent * -j * df;
            }

            frictionImpulseList[i] = frictionImpulse;
        }

        for (int i = 0; i < contactCount; i++)
        {
            FVector2 frictionImpulse = frictionImpulseList[i];
            FVector2 ra = raList[i];
            FVector2 rb = rbList[i];

            bodyA->linearVelocity += -frictionImpulse * bodyA->InvMass;
            bodyA->angularVelocity += -FVector2::CrossProduct(ra, frictionImpulse) * bodyA->InvInertia;
            bodyB->linearVelocity += frictionImpulse * bodyB->InvMass;
            bodyB->angularVelocity += FVector2::CrossProduct(rb, frictionImpulse) * bodyB->InvInertia;
        }
    }
};