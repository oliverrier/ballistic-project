#include "PCBullet.h"
#include "game/GameObjects/Bullet.h"
#include "game/scenes/GameScene.h"
#include <iostream>
#include "physicsEngine/common/Math.h"

#include "engine/Game/Game.h"
#include "engine/Scene/Scene.h"

PCBullet::PCBullet()
{
}

void PCBullet::updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene)
{
	Bullet& bullet = static_cast<Bullet&>(gameObject);
	GameScene& game_scene = static_cast<GameScene&>(scene);

	auto contactList = bullet.m_body->rb->GetContactList();
	if (contactList != nullptr && contactList->contact->GetManifold()->pointCount > 0) {
		auto world = World::GetWorld();
		world->DestroyBody(bullet.m_body->rb);
		Game::GetInstance()->GetCurrentScene()->RemoveGameObject(&bullet);
		game_scene.canShoot = true;
		return;
	}


	
	float windAngle = game_scene.windAngle * PI / 180;
	bullet.m_body->rb->ApplyForceToCenter(Vec2(std::cosf(windAngle) * game_scene.windForce, std::sinf(windAngle) * game_scene.windForce), true);


	bullet.m_circle.setPosition({ bullet.m_body->rb->GetPosition().x, bullet.m_body->rb->GetPosition().y });
}