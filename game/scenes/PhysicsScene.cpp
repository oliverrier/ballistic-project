#include "PhysicsScene.h"

#include <iostream>
#include "SFML/Graphics.hpp"


#include "game/GameObjects/UI/Button.h"
#include "game/Camera.h"
#include "game/EventManager.h"

#include "engine/core/Entity/CircleEntity.h"
#include "engine/core/Entity/EntityFactory.h"
#include "engine/core/Entity/SquareEntity.h"

PhysicsScene::PhysicsScene() : IScene(), 
                                m_world { PhysicsWorld::GetWorld() },
                                m_eventManager({ std::make_shared<sf::Window>(m_window) }), 
                                m_camera(Camera{ std::make_shared<sf::RenderWindow>(m_window) })
{
    initButtons();
    initObjects();
    registerEvents();
}

PhysicsScene::~PhysicsScene()
{
}

void PhysicsScene::initButtons()
{
    addGameObjects(new Button(1600, 50, 300, 100, "Leave hell",
        sf::Color(255, 0, 0), sf::Color(255, 75, 75), sf::Color(180, 0, 0),
        [&](Button* button) {m_window.close(); }));
}

void PhysicsScene::initObjects() 
{
    FVector2 S2_size = { 20.f,20.f };
    FVector2 S2_pos = { 200.f,100.f };
    FVector2 S2_vel = { 0.0f, 0.f };

    m_square2 = EntityFactory::create<SquareEntity>(S2_pos, S2_size, S2_vel, 0.1f);


    m_c = EntityFactory::create<CircleEntity>(FVector2{ 300.f, 200.f }, 10.f, S2_vel, 0.1f);

    m_sol = EntityFactory::create<SquareEntity>(FVector2{ 0.f, 500.f }, FVector2{ 800.f, 10.f }, FVector2{ 0.f, 0.f }, 0.1f, true);

}

void PhysicsScene::registerEvents()
{
    m_eventManager.addKeyPressedCallback(sf::Keyboard::Escape, [&](const sf::Event&) { m_window.close(); });

    sf::Vector2i lastMousePos;

    m_eventManager.addKeyPressedCallback(sf::Keyboard::W, [&](const sf::Event&) { m_camera.MoveUp(); });
    m_eventManager.addKeyPressedCallback(sf::Keyboard::S, [&](const sf::Event&) { m_camera.MoveDown(); });
    m_eventManager.addKeyPressedCallback(sf::Keyboard::A, [&](const sf::Event&) { m_camera.MoveLeft(); });
    m_eventManager.addKeyPressedCallback(sf::Keyboard::D, [&](const sf::Event&) { m_camera.MoveRight(); });

    /* move the camera */
    m_eventManager.addMousePressedCallback(sf::Mouse::Left, [&](const sf::Event& e)
        {
            m_camera.SetLastMousePosition({ static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y) });
        });

    m_eventManager.addEventCallback(sf::Event::MouseMoved, [&](const sf::Event&)
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                // Get the difference in mouse position since last frame
                sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);

                const FVector2 lastMousePosition = m_camera.getLastMousePosition();
                FVector2 delta = { mousePos.x - lastMousePosition.x, mousePos.y - lastMousePosition.y };

                m_camera.SetLastMousePosition({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) });
                // Move the view by the delta amount
                m_camera.Move(-delta.x, -delta.y);
            }
        });
}



void PhysicsScene::processInput(sf::Event& inputEvent)
{
    IScene::processInput(inputEvent);

    m_eventManager.processEvents();
}

void PhysicsScene::update(const float& deltaTime)
{
    IScene::update(deltaTime);
    // Handle events
    m_eventManager.processEvents();

    m_camera.Update();

    m_world->Update(deltaTime);

    // Clear the window
    m_window.clear();
    /*
    sf::RectangleShape s1(sf::Vector2f(20, 20));
    s1.setPosition(square1.get()->rb->position.x, square1.get()->rb->position.y);
    s1.setFillColor(sf::Color::Red);
    window.draw(s1);
    */

    sf::RectangleShape s2(sf::Vector2f(20, 20));
    s2.setOrigin(s2.getSize() / 2.f);
    s2.setPosition(m_square2.get()->rb->position.x, m_square2.get()->rb->position.y);
    s2.setFillColor(sf::Color::Green);
    m_window.draw(s2);

    sf::CircleShape cc(10);
    cc.setOrigin(cc.getRadius(), cc.getRadius());
    cc.setPosition(m_c.get()->rb->position.x, m_c.get()->rb->position.y);
    cc.setFillColor(sf::Color::Red);
    m_window.draw(cc);

    sf::RectangleShape sol1(sf::Vector2f(800, 10));
    sol1.setOrigin(sol1.getSize() / 2.f);
    sol1.setPosition(m_sol.get()->rb->position.x, m_sol.get()->rb->position.y);
    sol1.setFillColor(sf::Color::Blue);
    m_window.draw(sol1);

    // Display the window
    m_window.display();
}

void PhysicsScene::render()
{
    IScene::render();
}