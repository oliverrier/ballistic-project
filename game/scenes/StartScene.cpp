#include "StartScene.h"
#include <iostream>
#include "engine/Game/Game.h"
#include "game/Scenes/SceneEnum.h"
#include "engine/Ui/UiFactory.h"


StartScene::StartScene() : IScene()
{
    sf::Texture* backgroundTexture = new sf::Texture();
    if (!backgroundTexture->loadFromFile("Assets/Background.jpg")) {
        std::cout << "Error loading texture" << std::endl;
    }
    
    // Create background sprite
    m_backgroundSprite = new sf::Sprite();
    m_backgroundSprite->setTexture(*backgroundTexture);
    m_backgroundSprite->setPosition(0, 0);

    initButtons();
}

StartScene::~StartScene()
{
}

void start() {
    Game::GetInstance()->setCurrentScene(ScenesEnum::GAME_SCENE);
}

void StartScene::initButtons()
{
   /* addGameObjects(new Button(900, 400, 300, 100, "Welcome to hell",
        sf::Color(255, 0, 0), sf::Color(255, 75, 75), sf::Color(180, 0, 0),
        [&](Button* button) { Game::GetInstance()->setCurrentScene(ScenesEnum::PHYSICS_SCENE); }));
        */

    /*addGameObjects(new Button(900, 400, 300, 100, "Leave hell",
        sf::Color(255, 0, 0), sf::Color(255, 75, 75), sf::Color(180, 0, 0),
        [&](Button* button) {m_window.close(); }));*/

    startButton = UiFactory::create<RectangleButton>(FVector2(885.f, 480.f), FVector2(150.f, 50.f), sf::Color(92, 50, 18, 255), "Play");
    startButton->setOnClick(start);

    exitButton = UiFactory::create<RectangleButton>(FVector2(885.f, 640.f), FVector2(150.f, 50.f), sf::Color(92, 50, 18, 255), "Leave");
    exitButton->setOnClick([&]() {m_window->close(); });

    }

void StartScene::processInput(sf::Event& inputEvent)
{
    IScene::processInput(inputEvent);

    //if (inputEvent.key.code == sf::Keyboard::Escape)
    //    m_window.close();

    if (inputEvent.type == sf::Event::MouseButtonReleased && inputEvent.mouseButton.button == 0){

        FVector2 mousePosition = FVector2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
        startButton->handleClick(mousePosition);
        exitButton->handleClick(mousePosition);
    }
    
}

void StartScene::update(const float& deltaTime)
{
    startButton->setPosition(startButton->getInitialPosition());
    exitButton->setPosition(exitButton->getInitialPosition());
    IScene::update(deltaTime);
}

void StartScene::render()
{
    m_window->draw(*m_backgroundSprite);
    startButton->draw(*m_window, sf::RenderStates::Default);
    exitButton->draw(*m_window, sf::RenderStates::Default);

    IScene::render();
}