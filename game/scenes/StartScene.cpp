#include "StartScene.h"
#include <iostream>
#include "game/GameObjects/UI/Button.h"

StartScene::StartScene() : IScene()
{
    initButtons();
}

StartScene::~StartScene()
{
}

void StartScene::initButtons()
{
    addGameObjects(new Button(900, 400, 300, 100, "Welcome to hell",
        sf::Color(255, 0, 0), sf::Color(255, 75, 75), sf::Color(180, 0, 0),
        [&](Button* button) {m_window.close(); }));
    }

void StartScene::processInput(sf::Event& inputEvent)
{
    IScene::processInput(inputEvent);

    if (inputEvent.key.code == sf::Keyboard::Escape)
        m_window.close();
}

void StartScene::update(const float& deltaTime)
{
    IScene::update(deltaTime);
}

void StartScene::render()
{
    IScene::render();
}