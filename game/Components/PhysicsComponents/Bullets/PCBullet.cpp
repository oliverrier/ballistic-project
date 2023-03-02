#include "PCBullet.h"
#include "game/GameObjects/Bullet.h"
#include "game/scenes/GameScene.h"
#include <iostream>

PCBullet::PCBullet()
{
}

void PCBullet::updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene)
{
	Bullet& bullet = reinterpret_cast<Bullet&>(gameObject);
	GameScene& game_scene = reinterpret_cast<GameScene&>(gameObject);

	if (bullet.m_body->rb->GetContactList() != nullptr && bullet.m_destroyed == false) {

		std::cout << "destroyed" << std::endl;
		bullet.m_body->rb->SetLinearVelocity(Vec2(0, 0));
		bullet.m_body->rb->SetTransform(Vec2(0, -500), 0);
		bullet.m_body->rb->SetGravityScale(0.f);
		bullet.m_destroyed == true;
	}

	bullet.m_circle->setPosition({ bullet.m_body->rb->GetPosition().x, bullet.m_body->rb->GetPosition().y });
}