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


constexpr int window_width = 1920;
constexpr int window_height = 1080;

GameScene::GameScene() {

	sf::Texture* backgroundTexture = new sf::Texture();
	if (!backgroundTexture->loadFromFile("Assets/InGameBackGround.jpg")) {
		std::cout << "Error loading texture" << std::endl;
	}

	// Create background sprite
	m_backgroundSprite = new sf::Sprite();
	m_backgroundSprite->setTexture(*backgroundTexture);
	m_backgroundSprite->setPosition(0, 0);

	auto world = World::GetWorld();

	Fixture* m_character;
	std::shared_ptr<CircleEntity> body;

	m_platform = EntityFactory::create<RectEntity>(Vec2{ 300.0f, 55.f }, Vec2{ window_width / 2.f, 10.0f + 200.f }, BodyType::staticBody);


	m_eventManager = new EventManager(m_window);

	initButtons();
}

void goToMenu() {
	Game::GetInstance()->setCurrentScene(ScenesEnum::START_SCENE);
}

void GameScene::initButtons() {
	startButton = UiFactory::create<RectangleButton>(FVector2(885.f, 480.f), FVector2(150.f, 50.f), sf::Color(92, 50, 18, 255), "Retour au menu");
	startButton->setOnClick(goToMenu);

	exitButton = UiFactory::create<RectangleButton>(FVector2(885.f, 640.f), FVector2(150.f, 50.f), sf::Color(92, 50, 18, 255), "TEST");
	exitButton->setOnClick([&]() {m_window->close(); });
}

void GameScene::processInput(sf::Event& inputEvent) {
	IScene::processInput(inputEvent);

	//if (inputEvent.key.code == sf::Keyboard::Escape)
	//    m_window.close();

	if (inputEvent.type == sf::Event::MouseButtonReleased && inputEvent.mouseButton.button == 0) {

		FVector2 mousePosition = FVector2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
		startButton->handleClick(mousePosition);
		exitButton->handleClick(mousePosition);
	}

}

void GameScene::update(const float& deltaTime) {

	//startButton->setPosition(startButton->getInitialPosition());
	//exitButton->setPosition(exitButton->getInitialPosition());



	IScene::update(deltaTime);
}

void GameScene::render() {
	m_window->draw(*m_backgroundSprite);
	//startButton->draw(*m_window, sf::RenderStates::Default);
	//exitButton->draw(*m_window, sf::RenderStates::Default);
	sf::RectangleShape floor{ { 30.0f, 5.f } };
	floor.setPosition({ m_platform->rb->GetPosition().x, m_platform->rb->GetPosition().y });
	m_window->draw(floor);
	IScene::render();
}