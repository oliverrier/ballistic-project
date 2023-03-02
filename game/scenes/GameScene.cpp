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

	pannel = UiFactory::create < HudElement<std::string>>(FVector2(0.f, 780.f), FVector2(1920.f, 300.f), sf::Color(19, 48, 54, 255), "", "Assets/WoodenTexture.png");
	hudElements.push_back(pannel);
	moveInfo = UiFactory::create < HudElement<std::string>>(FVector2(200.f, 880.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Q,D	Deplacements");
	hudElements.push_back(moveInfo);
	aimInfo = UiFactory::create < HudElement<std::string>>(FVector2(750.f, 880.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Z,S	Viser");
	hudElements.push_back(aimInfo);
	shootInfo = UiFactory::create < HudElement<std::string>>(FVector2(1300.f, 880.f), FVector2(500.f, 50.f), sf::Color(19, 48, 54, 255), "Space  Tirer");
	hudElements.push_back(shootInfo);



	//sf::Texture* backgroundTexture = new sf::Texture();
	//if (!backgroundTexture->loadFromFile("Assets/InGameBackGround.jpg")) {
	//	std::cout << "Error loading texture" << std::endl;
	//}

	//// Create background sprite
	//m_backgroundSprite = new sf::Sprite();
	//m_backgroundSprite->setTexture(*backgroundTexture);
	//m_backgroundSprite->setPosition(0, 0);

	m_world = World::GetWorld();

	Fixture* m_character;

	m_platform = EntityFactory::create<RectEntity>(Vec2{ 300.0f, 55.f }, Vec2{ window_width / 2.f, 10.0f + 200.f }, BodyType::staticBody);
	floor = new sf::RectangleShape(sf::Vector2f(300.0f, 55.f));

	body = EntityFactory::create<CircleEntity>(5.f, Vec2{ window_width / 2.f, 10.0f }, BodyType::dynamicBody);
	m_circle = new sf::CircleShape(body->radius);

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

	if (inputEvent.type == sf::Event::KeyPressed && inputEvent.key.code == sf::Keyboard::Z) {
		//body->rb->SetAwake(true);
		body->rb->SetFixedRotation(true);
		body->rb->SetGravityScale(1.f);
		body->rb->SetAngularVelocity(0.f);
		body->rb->SetTransform(Vec2(50.f, 600.f), 0.f);
		body->rb->SetLinearVelocity(Vec2(0.5f * 600, -0.8f * 600));

			//m_littleBox->SetAwake(true);
			//m_littleBox->SetGravityScale(1);
			//m_littleBox->SetAngularVelocity(0);
			//m_littleBox->SetTransform(m_launcherBody->GetWorldPoint(b2Vec2(3, 0)), m_launcherBody->GetAngle());
			//m_littleBox->SetLinearVelocity(m_launcherBody->GetWorldVector(b2Vec2(m_launchSpeed, 0)));
			//m_firing = true;
		body->rb->ApplyForceToCenter(Vec2(100.f, -100.f), true);

	}

	if (inputEvent.type == sf::Event::MouseButtonReleased && inputEvent.mouseButton.button == 0) {

		/*FVector2 mousePosition = FVector2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
		startButton->handleClick(mousePosition);
		exitButton->handleClick(mousePosition);*/
	}

}

void GameScene::update(const float& deltaTime) {

	//startButton->setPosition(startButton->getInitialPosition());
	//exitButton->setPosition(exitButton->getInitialPosition
	if (body->rb->GetContactList() != nullptr) {
		body->rb->SetLinearVelocity(Vec2(0,0));
		body->rb->SetTransform(Vec2(0, -500), 0);
		body->rb->SetGravityScale(0.f);
	}
	float timeStep = 1.0f / 60.0f;

	int velocityIterations = 6;
	int positionIterations = 2;

	m_world->Step(timeStep, velocityIterations, positionIterations);

	for (auto element : hudElements)
	{
		element->setPosition(element->getInitialPosition());
	}

	floor->setPosition({ m_platform->rb->GetPosition().x, m_platform->rb->GetPosition().y });
	m_circle->setPosition({ body->rb->GetPosition().x, body->rb->GetPosition().y });

	

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
	
	m_window->draw(*floor);
	m_window->draw(*m_circle);
	IScene::render();
}