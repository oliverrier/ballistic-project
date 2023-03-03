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

		ApplyDamage(game_scene, bulletPosition, isFrag);

		auto world = World::GetWorld();
		world->DestroyBody(bullet.m_body->rb);
		game_scene.RemoveGameObject(&bullet);

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

void PCBullet::ApplyDamage(IScene& scene, Vec2 bulletPos, bool isFrag)
{
	GameScene& game_scene = static_cast<GameScene&>(scene);

	// get distance with players;
	auto p1_pos = game_scene.player1->m_body->rb->GetPosition();
	auto dist_1 = FVector2::Distance({ bulletPos.x, bulletPos.y }, { p1_pos.x, p1_pos.y });

	auto min_damage = isFrag ? 5.f : 25.f;
	auto max_damage = isFrag ? 50.f : 10.f;
	auto dist = isFrag ? 50.f : 100.f;
	if (dist_1 <= dist) {
		auto damage = MapValue(dist_1, 0.f, 100.f, max_damage, min_damage);
		game_scene.player1->takeDamage(damage);
	}

	auto p2_pos = game_scene.player2->m_body->rb->GetPosition();
	auto dist_2 = FVector2::Distance({ bulletPos.x, bulletPos.y }, { p2_pos.x, p2_pos.y });
	if (dist_2 <= dist) {
		auto damage = MapValue(dist_2, 0.f, 100.f, max_damage, min_damage);
		game_scene.player2->takeDamage(damage);
	}
}