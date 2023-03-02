#include "PCBullet.h"
#include "game/GameObjects/Bullet.h"
#include "game/scenes/GameScene.h"
#include <iostream>

#include "engine/Game/Game.h"
#include "engine/Scene/Scene.h"

PCBullet::PCBullet()
{
}

void PCBullet::updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene)
{
	Bullet& bullet = static_cast<Bullet&>(gameObject);

	if (bullet.m_body->rb->GetContactList() != nullptr) {
		std::cout << "destroyed" << std::endl;
		auto world = World::GetWorld();
		world->DestroyBody(bullet.m_body->rb);
		Game::GetInstance()->GetCurrentScene()->RemoveGameObject(&bullet);
		return;
	}

	bullet.m_circle.setPosition({ bullet.m_body->rb->GetPosition().x, bullet.m_body->rb->GetPosition().y });
}