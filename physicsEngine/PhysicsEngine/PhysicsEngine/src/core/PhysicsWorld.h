#pragma once
#include <memory>
#include <vector>

#include "CollisionResolver.h"
#include "RigidBody.h"


class PhysicsWorld
{
private:
	PhysicsWorld() = default;

public:
	void Update(const float deltaTime) const
	{
		for (auto& body : m_rigidBodies)
		{
			ApplyGravity(body);
			UpdateObject(body, deltaTime);
		}

		for (unsigned int i = 0; i < m_rigidBodies.size(); i++) {
			for (unsigned int j = i + 1; j < m_rigidBodies.size(); j++) {
				const auto a = m_rigidBodies[i];
				const auto b = m_rigidBodies[j];

				if (CollisionResolver::IsColliding(a.get(), b.get())) {
					CollisionResolver::ResolveCollision(a.get(), b.get());
				}
			}
		}
	}

private:
	void ApplyGravity(const std::shared_ptr<RigidBody>& body) const
	{
		if (!body->isStatic) {
			body->AddForce(m_gravity * body->mass);
		}
	}

	void UpdateObject(const std::shared_ptr<RigidBody>& body, const float deltaTime) const
	{
		body->Update(deltaTime);
	}
	
public:
	void AddRigidBody(std::shared_ptr<RigidBody>& rigidBody)
	{
		m_rigidBodies.push_back(rigidBody);
	}

	void RemoveRigidBody(std::shared_ptr<RigidBody> rigidBody)
	{
		const auto it = std::ranges::find(m_rigidBodies, rigidBody);
		if (it != m_rigidBodies.end()) {
			m_rigidBodies.erase(it);
		}
	}


	static std::shared_ptr<PhysicsWorld> GetWorld()
	{
		static std::shared_ptr<PhysicsWorld> instance{ new PhysicsWorld };
		return instance;
	}


private:
	std::vector<std::shared_ptr<RigidBody>> m_rigidBodies;
	FVector2 m_gravity = FVector2::Up * 9.81f;
};

