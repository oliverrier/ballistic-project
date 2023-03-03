#include "GameScene.h"
#include "engine/Game/Game.h"
#include "game/Scenes/SceneEnum.h"
#include "engine/Ui/UiFactory.h"

#include <chrono>
#include <iostream> 

#include "physicsEngine/dynamics/World.h"
#include "SFML/Graphics.hpp"
#include "engine/Entity/Entity.h"
#include "engine/Entity/CircleEntity.h"
#include "engine/Entity/RectEntity.h"
#include "game/GameObjects/Bullet.h"
#include <game/GameObjects/GameObjectFactory.h>
#include <game/Utils/Utils.h>
#include "game/GameObjects/Character/Character.h"


constexpr int window_width = 1920;
constexpr int window_height = 1080;

GameScene::GameScene() {

	windAngle = -45.f;
	windForce = 60.f;
	shootingAngle = -90.f;

	pannel = UiFactory::create < HudElement<std::string>>(FVector2(0.f, 780.f), FVector2(1920.f, 300.f), sf::Color(19, 48, 54, 255), "", "Assets/WoodenTexture.png");
	hudElements.push_back(pannel);
	moveInfo = UiFactory::create < HudElement<std::string>>(FVector2(200.f, 980.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Q,D	Deplacements");
	hudElements.push_back(moveInfo);
	aimInfo = UiFactory::create < HudElement<std::string>>(FVector2(750.f, 980.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Z,S	Viser");
	hudElements.push_back(aimInfo);
	shootInfo = UiFactory::create < HudElement<std::string>>(FVector2(1300.f, 980.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Space  Tirer");
	hudElements.push_back(shootInfo);
	windArrow = UiFactory::create < HudArrow>(FVector2(420.f, 200.f), FVector2(150.f, 50.f), &windAngle);
	timer = UiFactory::create < HudElement<std::string>>(FVector2(window_width / 2.f, 100.f), FVector2(50.f, 50.f), sf::Color::Transparent, "50");
	hudElements.push_back(timer);
	playerInfo = UiFactory::create < HudElement<std::string>>(FVector2(window_width / 2.f - 65.f, 150.f), FVector2(100.f, 50.f), sf::Color::Transparent, "Player 1");
	hudElements.push_back(playerInfo);

	time = 30.f;

	const Vec2 character_1_start_pos = { 150.f, window_height - 500 };
	player1 = GameObjectFactory::create<Character>(character_1_start_pos, sf::Keyboard::Q, sf::Keyboard::D, 0);

	addGameObjects(player1);

	Vec2 character_2_start_pos = { window_width - 150, window_height - 500 };
	player2 = GameObjectFactory::create<Character>(character_2_start_pos, sf::Keyboard::Left, sf::Keyboard::Right, 1);

	addGameObjects(player2);

	m_world = World::GetWorld();

	std::vector<Vec2> vertices;
	GenerateFloorVertex({ 0.f, 200.f }, { window_width , 200.f }, 10, vertices);
	m_platform = GameObjectFactory::create<Ground>(vertices, Vec2(0, window_height - 200));
	addGameObjects(m_platform);


	m_currentCharacter = player1;

	initButtons();
}

void goToMenu() {
	Game::GetInstance()->setCurrentScene(ScenesEnum::START_SCENE);
}

void GameScene::initButtons() {
	/*startButton = UiFactory::create<RectangleButton>(FVector2(885.f, 480.f), FVector2(150.f, 50.f), sf::Color(92, 50, 18, 255), "Retour au menu");
	startButton->setOnClick(goToMenu);

	exitButton = UiFactory::create<RectangleButton>(FVector2(885.f, 640.f), FVector2(150.f, 50.f), sf::Color(92, 50, 18, 255), "TEST");
	exitButton->setOnClick([&]() {m_window->close(); });*/
}

void GameScene::processInput(sf::Event& inputEvent) {
	IScene::processInput(inputEvent);

	if (canShoot == true && inputEvent.KeyPressed && inputEvent.key.code == sf::Keyboard::Space) {

		canShoot = false;
		std::shared_ptr<Bullet> bullet = GameObjectFactory::create<Bullet>(window_width, shootingAngle);
		float angleToShoot = bullet->angle * PI / 180;

		bullet->m_body->rb->SetFixedRotation(true);
		bullet->m_body->rb->SetGravityScale(1.f);
		bullet->m_body->rb->SetAngularVelocity(0.f);
		bullet->m_body->rb->SetTransform(Vec2(100.f, 600.f), 0.f);
		bullet->m_body->rb->SetLinearVelocity(Vec2(std::cosf(angleToShoot) * 600, std::sinf(angleToShoot) * 600));
		addGameObjects(bullet);
	}

	if (inputEvent.key.code == sf::Keyboard::Z) {
		shootingAngle -= 1;
	}
	else if (inputEvent.key.code == sf::Keyboard::S)
	{
		shootingAngle += 1;
	}

}

void GameScene::update(const float& deltaTime) {

	time -= deltaTime / 10.f;

	if (time <= 0.f)
	{
		is_player_1_turn = !is_player_1_turn;
		player_index_to_play = is_player_1_turn ? 0 : 1;

		playerInfo->m_textDisplayed.setString(is_player_1_turn ? "Player 1" : "Player 2");
		time = 30.f;

		if (player_index_to_play == 0)
		{
			m_currentCharacter = player1;
		} else
		{
			m_currentCharacter = player2;
		}

	}

	timer->m_textDisplayed.setString(std::to_string((int)time));

	int velocityIterations = 6;
	int positionIterations = 2;

	m_world->Step(deltaTime, velocityIterations, positionIterations);

	/*
	for (auto element : hudElements)
	{
		element->setPosition(element->getInitialPosition());
	}

	windArrow->setPosition(windArrow->getInitialPosition());
	*/

	auto currentCharacterContactList = m_currentCharacter->m_body->rb->GetContactList();
	if (!m_currentCharacter->m_startJumping && currentCharacterContactList != nullptr && currentCharacterContactList->contact->GetManifold()->pointCount > 0) {
		m_currentCharacter->m_isJumping = false;
	}

	IScene::update(deltaTime);
}

void GameScene::render() {
	//m_window->draw(*m_backgroundSprite);
	
	//startButton->draw(*m_window, sf::RenderStates::Default);
	//exitButton->draw(*m_window, sf::RenderStates::Default);
	
	IScene::render();

	for (auto element : hudElements)
	{
		m_window->draw(*element);
	}

	m_window->draw(*windArrow);

}