#include "ICCharacter.h"

#include "game/GameObjects/Character/Character.h"
#include "game/Scenes/GameScene.h"

ICCharacter::ICCharacter()
{
}

void ICCharacter::processInputImplementation(IGameObject& gameObject, sf::Event& inputEvent, IScene& scene)
{
	Character& character = static_cast<Character&>(gameObject);
	GameScene& game_scene = static_cast<GameScene&>(scene);

	if (character.index != game_scene.player_index_to_play) return;

	if (inputEvent.type == sf::Event::KeyPressed && inputEvent.key.code == character.left) {
		//body->rb->SetAwake(true);

		character.m_body->rb->SetFixedRotation(true);
		character.m_body->rb->SetLinearVelocity(Vec2{ -10.f , -1.f});
	}
	else if (inputEvent.type == sf::Event::KeyPressed && inputEvent.key.code == character.right) {
		//body->rb->SetAwake(true);

		character.m_body->rb->SetFixedRotation(true);
		character.m_body->rb->SetLinearVelocity(Vec2{ 10.f , -1.f });
	}
	if (inputEvent.type == sf::Event::KeyReleased) 
	{
		character.m_body->rb->SetLinearVelocity(Vec2{ 0.f , 5.f });
		if (inputEvent.key.code == sf::Keyboard::Enter)
		{
			character.m_startJumping = false;
		}
	}
	if (!character.m_isJumping && inputEvent.type == sf::Event::KeyPressed && inputEvent.key.code == sf::Keyboard::Enter) {
		//body->rb->SetAwake(true);
		character.m_isJumping = true;
		character.m_startJumping = true;
		character.m_body->rb->SetFixedRotation(true);
		character.m_body->rb->SetLinearVelocity(Vec2{ 0.f , -200.f });

	}
}
