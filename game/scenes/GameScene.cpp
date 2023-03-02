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

	pannel = UiFactory::create < HudElement<std::string>>(FVector2(0.f, 780.f), FVector2(1920.f, 300.f), sf::Color(19, 48, 54, 255), "", "Assets/WoodenTexture.png");
	hudElements.push_back(pannel);
	moveInfo = UiFactory::create < HudElement<std::string>>(FVector2(200.f, 880.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Q,D	Deplacements");
	hudElements.push_back(moveInfo);
	aimInfo = UiFactory::create < HudElement<std::string>>(FVector2(750.f, 880.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Z,S	Viser");
	hudElements.push_back(aimInfo);
	shootInfo = UiFactory::create < HudElement<std::string>>(FVector2(1300.f, 880.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Space  Tirer");
	hudElements.push_back(shootInfo);

	angle1 = 50.f;
	angle2 = 200.f;

	std::shared_ptr<Bullet> mainBullet = GameObjectFactory::create<Bullet>(window_width, angle1);
	addGameObjects(mainBullet);

	//addGameObjects(GameObjectFactory::create<Bullet>(body2, circle2, angle2));


	m_world = World::GetWorld();

	Fixture* m_character;

	std::vector<Vec2> vertices;
	GenerateFloorVertex({ 0.f, 200.f }, { window_width , 200.f }, 10, vertices);
	m_platform = GameObjectFactory::create<Ground>(vertices, Vec2(0, window_height - 200));
	addGameObjects(m_platform);
	
	m_eventManager = new EventManager(m_window);

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

	//if (inputEvent.key.code == sf::Keyboard::Escape)
	//    m_window.close();

	

	if (inputEvent.type == sf::Event::MouseButtonReleased && inputEvent.mouseButton.button == 0) {

		/*FVector2 mousePosition = FVector2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
		startButton->handleClick(mousePosition);
		exitButton->handleClick(mousePosition);*/
	}

}

void GameScene::update(const float& deltaTime) {

	//startButton->setPosition(startButton->getInitialPosition());
	//exitButton->setPosition(exitButton->getInitialPosition
	/*if (body->rb->GetContactList() != nullptr) {
		body->rb->SetLinearVelocity(Vec2(0,0));
		body->rb->SetTransform(Vec2(0, -500), 0);
		body->rb->SetGravityScale(0.f);
	}*/
	float timeStep = 1.0f / 60.0f;

	int velocityIterations = 6;
	int positionIterations = 2;

	m_world->Step(timeStep, velocityIterations, positionIterations);

	for (auto element : hudElements)
	{
		element->setPosition(element->getInitialPosition());
	}


	IScene::update(deltaTime);
}

void GameScene::render() {
	//m_window->draw(*m_backgroundSprite);
	

	for (auto element : hudElements)
	{
		m_window->draw(*element);
	}

	//startButton->draw(*m_window, sf::RenderStates::Default);
	//exitButton->draw(*m_window, sf::RenderStates::Default);
	
	IScene::render();
}