#include "ICBullet.h"
#include "game/GameObjects/Bullet.h"
#include "game/scenes/GameScene.h"

ICBullet::ICBullet()
{
}

void ICBullet::processInputImplementation(IGameObject& gameObject, sf::Event& inputEvent, IScene& scene)
{
	Bullet& bullet= reinterpret_cast<Bullet&>(gameObject);
	GameScene& game_scene = reinterpret_cast<GameScene&>(gameObject);


	if (inputEvent.type == sf::Event::KeyPressed && inputEvent.key.code == sf::Keyboard::Z) {
		//body->rb->SetAwake(true);
		std::cout << bullet.angle << std::endl;

		bullet.m_body->rb->SetFixedRotation(true);
		bullet.m_body->rb->SetGravityScale(1.f);
		bullet.m_body->rb->SetAngularVelocity(0.f);
		bullet.m_body->rb->SetTransform(Vec2(bullet.angle, 600.f), 0.f);
		bullet.m_body->rb->SetLinearVelocity(Vec2( 0.5f * 600, -0.8f * 600));
	}
}