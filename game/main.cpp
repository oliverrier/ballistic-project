#include <iostream>

#include "engine/core/PhysicsWorld.h"
#include "engine/core/Entity/CircleEntity.h"
#include "engine/core/Entity/EntityFactory.h"
#include "engine/core/Entity/SquareEntity.h"
#include "SFML/Graphics.hpp"
#include "Camera.h"
#include "EventManager.h"

int main() {



    auto world = PhysicsWorld::GetWorld();

    /*

    // square 1
    FVector2 S1_size = { 20.f,20.f };
    FVector2 S1_pos = { 100.f,100.f };
    FVector2 S1_vel = { 0.f, 0.f };

    auto square1 = EntityFactory::create<SquareEntity>(S1_pos, S1_size, S1_vel, 0.1f, true);
    */
    // square 2
    FVector2 S2_size = { 20.f,20.f };
    FVector2 S2_pos = { 200.f,100.f };
    FVector2 S2_vel = { 0.0f, 0.f };

    auto square2 = EntityFactory::create<SquareEntity>(S2_pos, S2_size, S2_vel, 0.1f);


    auto c = EntityFactory::create<CircleEntity>(FVector2{ 300.f, 200.f }, 10.f, S2_vel, 0.1f);

    auto sol = EntityFactory::create<SquareEntity>(FVector2{ 0.f, 500.f }, FVector2{ 800.f, 10.f }, FVector2{ 0.f, 0.f }, 0.1f, true);

    // square 2
    std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(sf::VideoMode(800, 600), "Drawing Squares");

    EventManager eventManager = { window };

    Camera camera = Camera{ window };

    sf::Vector2i lastMousePos;

    eventManager.addKeyPressedCallback(sf::Keyboard::W, [&](const sf::Event&) { camera.MoveUp(); });
    eventManager.addKeyPressedCallback(sf::Keyboard::S, [&](const sf::Event&) { camera.MoveDown(); });
    eventManager.addKeyPressedCallback(sf::Keyboard::A, [&](const sf::Event&) { camera.MoveLeft(); });
    eventManager.addKeyPressedCallback(sf::Keyboard::D, [&](const sf::Event&) { camera.MoveRight(); });

    /* move the camera */
    eventManager.addMousePressedCallback(sf::Mouse::Left, [&](const sf::Event& e)
    {
    	camera.SetLastMousePosition({ static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y) });
    });

    eventManager.addEventCallback(sf::Event::MouseMoved, [&](const sf::Event&)
    {
	    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	    {
            // Get the difference in mouse position since last frame
            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);

            const FVector2 lastMousePosition = camera.getLastMousePosition();
            FVector2 delta = { mousePos.x - lastMousePosition.x, mousePos.y - lastMousePosition.y };

            camera.SetLastMousePosition({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) });
            // Move the view by the delta amount
            camera.Move(-delta.x, -delta.y);
	    }
    });


    // Start the main event loop
    while (window->isOpen())
    {
        // Handle events
        eventManager.processEvents();

        camera.Update();

        world->Update(0.01f);


        // Clear the window
        window->clear();
        /*
        sf::RectangleShape s1(sf::Vector2f(20, 20));
        s1.setPosition(square1.get()->rb->position.x, square1.get()->rb->position.y);
        s1.setFillColor(sf::Color::Red);
        window.draw(s1);
        */

        sf::RectangleShape s2(sf::Vector2f(20, 20));
        s2.setOrigin(s2.getSize() / 2.f);
        s2.setPosition(square2.get()->rb->position.x, square2.get()->rb->position.y);
        s2.setFillColor(sf::Color::Green);
        window->draw(s2);

        sf::CircleShape cc(10);
        cc.setOrigin(cc.getRadius(), cc.getRadius());
        cc.setPosition(c.get()->rb->position.x, c.get()->rb->position.y);
        cc.setFillColor(sf::Color::Red);
        window->draw(cc);

        sf::RectangleShape sol1(sf::Vector2f(800, 10));
        sol1.setOrigin(sol1.getSize() / 2.f);
        sol1.setPosition(sol.get()->rb->position.x, sol.get()->rb->position.y);
        sol1.setFillColor(sf::Color::Blue);
        window->draw(sol1);

        // Display the window
        window->display();
    }

    return 0;
}