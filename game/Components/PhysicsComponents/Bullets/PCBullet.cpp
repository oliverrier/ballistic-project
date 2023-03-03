#include "PCBullet.h"
#include "game/GameObjects/Bullet.h"
#include "game/scenes/GameScene.h"
#include <iostream>
#include "physicsEngine/common/Math.h"

#include "engine/Game/Game.h"
#include "engine/Scene/Scene.h"
#include "game/GameObjects/GameObjectFactory.h"
#include "game/Utils/Utils.h"

PCBullet::PCBullet()
{
}

void PCBullet::updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene)
{
	Bullet& bullet = static_cast<Bullet&>(gameObject);
	GameScene& game_scene = static_cast<GameScene&>(scene);

	auto contactList = bullet.m_body->rb->GetContactList();
	if (contactList != nullptr && contactList->contact->GetManifold()->pointCount > 0) {
	  	auto bulletPosition = bullet.m_body->rb->GetPosition();
		bool isFrag = bullet.is_fragmentation;
		auto world = World::GetWorld();
		world->DestroyBody(bullet.m_body->rb);
		Game::GetInstance()->GetCurrentScene()->RemoveGameObject(&bullet);

		if (!isFrag) {
			for (int i = 0; i < 4; ++i)
			{
				float angle = MapValue((float)rand() / RAND_MAX, 0.f, 1.f, -180.f, 0.f);
				float angleToShoot = (- 70 - 10 * i)* PI / 180;

				auto b = GameObjectFactory::create<Bullet>(angle, Vec2(bulletPosition.x - 2 * i, bulletPosition.y - 10 * i), true);
				b->m_body->rb->SetLinearVelocity(Vec2(std::cosf(angleToShoot) * 100, std::sinf(angleToShoot) * 200));

				game_scene.m_fragmentation.push_back(GameObjectFactory::create<Bullet>(angle, bulletPosition, true));
				game_scene.addGameObjects(b);
			}
		}

		return;
	}


	
	float windAngle = game_scene.windAngle * PI / 180;
	bullet.m_body->rb->ApplyForceToCenter(Vec2(std::cosf(windAngle) * game_scene.windForce, std::sinf(windAngle) * game_scene.windForce), true);


	bullet.m_circle.setPosition({ bullet.m_body->rb->GetPosition().x, bullet.m_body->rb->GetPosition().y });
}