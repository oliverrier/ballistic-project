#include "ICBullet.h"
#include "game/GameObjects/Bullet.h"
#include "game/scenes/GameScene.h"
#include "physicsEngine/common/Math.h"

ICBullet::ICBullet()
{
}

void ICBullet::processInputImplementation(IGameObject& gameObject, sf::Event& inputEvent, IScene& scene)
{
	Bullet& bullet= reinterpret_cast<Bullet&>(gameObject);
	GameScene& game_scene = reinterpret_cast<GameScene&>(gameObject);
	float angleToShoot = *bullet.m_angle * PI / 180;

	if (inputEvent.type == sf::Event::KeyPressed && inputEvent.key.code == sf::Keyboard::Space) {

		bullet.m_destroyed = false;
		bullet.m_body->rb->SetFixedRotation(true);
		bullet.m_body->rb->SetGravityScale(1.f);
		bullet.m_body->rb->SetAngularVelocity(0.f);
		bullet.m_body->rb->SetTransform(Vec2(100.f, 600.f), 0.f);
		bullet.m_body->rb->SetLinearVelocity(Vec2(std::cosf(angleToShoot) * 600, std::sinf(angleToShoot) * 600));
	}
}

//angle = angle * Math.PI / 180; // if you're using degrees instead of radians
//return[length * Math.cos(angle) + xCoord, length * Math.sin(angle) + yCoord]
