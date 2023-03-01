#include "GameScene.h"
#include "engine/Game/Game.h"
#include "game/Scenes/SceneEnum.h"
#include "engine/Ui/UiFactory.h"

#include <iostream> 

GameScene::GameScene() {

    sf::Texture* backgroundTexture = new sf::Texture();
    if (!backgroundTexture->loadFromFile("Assets/InGameBackGround.jpg")) {
        std::cout << "Error loading texture" << std::endl;
    }

    // Create background sprite
    m_backgroundSprite = new sf::Sprite();
    m_backgroundSprite->setTexture(*backgroundTexture);
    m_backgroundSprite->setPosition(0, 0);

    m_eventManager = new EventManager(m_window);

    initButtons();
}

void goToMenu() {
    Game::GetInstance()->setCurrentScene(ScenesEnum::START_SCENE);
}

void GameScene::initButtons() {
    startButton = UiFactory::create<RectangleButton>(FVector2(885.f, 480.f), FVector2(150.f, 50.f), sf::Color(92, 50, 18, 255), "Retour au menu");
    startButton->setOnClick(goToMenu);

    exitButton = UiFactory::create<RectangleButton>(FVector2(885.f, 640.f), FVector2(150.f, 50.f), sf::Color(92, 50, 18, 255), "Leave");
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

void GameScene::update(const float& deltaTime){

    startButton->setPosition(startButton->getInitialPosition());
    exitButton->setPosition(exitButton->getInitialPosition());
    IScene::update(deltaTime);
}

void GameScene::render() {
    m_window->draw(*m_backgroundSprite);
    startButton->draw(*m_window, sf::RenderStates::Default);
    exitButton->draw(*m_window, sf::RenderStates::Default);

    IScene::render();
}