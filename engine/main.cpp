#include "core/PhysicsWorld.h"
#include "core/Entity/CircleEntity.h"
#include "core/Entity/EntityFactory.h"
#include "core/Entity/SquareEntity.h"
#include "SFML/Graphics.hpp"

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

    sf::RenderWindow window(sf::VideoMode(800, 600), "Drawing Squares");

    // Start the main event loop
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        world->Update(0.01f);


        // Clear the window
        window.clear();
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
        window.draw(s2);

        sf::CircleShape cc(10);
        cc.setOrigin(cc.getRadius(), cc.getRadius());
        cc.setPosition(c.get()->rb->position.x, c.get()->rb->position.y);
        cc.setFillColor(sf::Color::Red);
        window.draw(cc);

        sf::RectangleShape sol1(sf::Vector2f(800, 10));
        sol1.setOrigin(sol1.getSize() / 2.f);
        sol1.setPosition(sol.get()->rb->position.x, sol.get()->rb->position.y);
        sol1.setFillColor(sf::Color::Blue);
        window.draw(sol1);

        // Display the window
        window.display();
    }

    return 0;
}