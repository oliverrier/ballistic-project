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
#include "game/GameObjects/Bullet.h"
#include <game/GameObjects/GameObjectFactory.h>
#include <game/Utils/Utils.h>


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

	m_world = World::GetWorld();

	Fixture* m_character;

	std::vector<Vec2> vertices;
	GenerateFloorVertex({ 0.f, 200.f }, { window_width , 200.f }, 10, vertices);
	m_platform = GameObjectFactory::create<Ground>(vertices, Vec2(0, window_height - 200));
	addGameObjects(m_platform);
	
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

	float timeStep = 1.0f / 60.0f;

	int velocityIterations = 6;
	int positionIterations = 2;

	m_world->Step(timeStep, velocityIterations, positionIterations);

	for (auto element : hudElements)
	{
		element->setPosition(element->getInitialPosition());
	}

	windArrow->setPosition(windArrow->getInitialPosition());

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