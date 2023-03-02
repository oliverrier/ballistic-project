#include "PCBullet.h"
#include "game/GameObjects/Bullet.h"
#include <iostream>

PCBullet::PCBullet()
{
}

void PCBullet::updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene)
{
	Bullet& bullet = reinterpret_cast<Bullet&>(gameObject);

	if (bullet.m_body->rb->GetContactList() != nullptr) {
		std::cout << "destroyed" << std::endl;
		bullet.m_body->rb->SetLinearVelocity(Vec2(0, 0));
		bullet.m_body->rb->SetTransform(Vec2(0, -500), 0);
		bullet.m_body->rb->SetGravityScale(0.f);
	}

	bullet.m_circle->setPosition({ bullet.m_body->rb->GetPosition().x, bullet.m_body->rb->GetPosition().y });
}