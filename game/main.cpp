#include <iostream>
#include <chrono>
#include <engine/Game/Game.h>

#include "Scenes/StartScene.h"
#include "scenes/GameScene.h"
#include "Scenes/SceneEnum.h"


int main()
{
    Game* game = Game::GetInstance();
    game->addScenes(new StartScene());
    game->addScenes(new GameScene());
    //game->addScenes(new PhysicsScene());

    game->setCurrentScene(ScenesEnum::GAME_SCENE);

    game->run(sf::VideoMode(1920, 1080), "ballistic-project", sf::Style::Resize);

    return 0;
}


// #include <chrono>
// #include <iostream>

// #include "physicsEngine/dynamics/World.h"
// #include "SFML/Graphics.hpp"
// #include "engine/Entity/Entity.h"
// #include "engine/Entity/RectEntity.h"
// #include "engine/Entity/CircleEntity.h"
// #include "engine/Entity/PolygonEntity.h"
// #include <lib/thor/include/Thor/Shapes/ConcaveShape.hpp>

// #include "EventManager.h"
// #include "Utils/Utils.h"

// constexpr int window_width = 800;
// constexpr int window_height = 600;

// int main()
// {
//     auto world = World::GetWorld();

//     Fixture* m_character;
//     std::shared_ptr<CircleEntity> body;
//     std::shared_ptr<PolygonEntity> poly;
//     std::shared_ptr<RectEntity> gr;

//     // Actor
//     {
//         body = EntityFactory::create<CircleEntity>(10.f, Vec2{ window_width / 2.f, 10.0f }, BodyType::dynamicBody);
//     }

//     {
//         std::vector<Vec2> vs;
//         GenerateFloorVertex({ 0.f, 200.f }, { window_width , 200.f }, 10, vs);
//         poly = EntityFactory::create<PolygonEntity>(vs, Vec2(0, window_height));
//     }

//     float timeStep = 1.0f / 60.0f;

//     int velocityIterations = 6;
//     int positionIterations = 2;

//     const std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(sf::VideoMode(window_width, window_height), "Drawing Squares");

//     EventManager evm = { window, false };

//     evm.addEventCallback(sf::Event::MouseButtonPressed, [&](const sf::Event& e)
//     {
//             if (e.mouseButton.button == sf::Mouse::Left) {
//                 float x = e.mouseButton.x;
//                 float y = e.mouseButton.y;
//                 body->rb->SetTransform({ x, y }, 0.f);
//                 body->rb->SetLinearVelocity({ 0.f, 0.f });
//             }
//     });

//     evm.addEventCallback(sf::Event::EventType::Closed, [&](const sf::Event&) { window->close(); });

//     while (window->isOpen())
//     {
//         evm.processEvents();
//         world->Step(timeStep, velocityIterations, positionIterations);

//         window->clear();

//         sf::CircleShape circle{ body->radius };
//         circle.setPosition({ body->rb->GetPosition().x, body->rb->GetPosition().y });
//         circle.setOrigin({ body->radius, body->radius });
//         circle.setFillColor(sf::Color::Red);
//         window->draw(circle);

//         /*
//         sf::RectangleShape rectangle{ { gr->size.x, gr->size.y } };
//         rectangle.setPosition({ gr->rb->GetPosition().x, gr->rb->GetPosition().y });
//         rectangle.setOrigin(rectangle.getSize() / 2.f);
//         window->draw(rectangle);
//         */

//         thor::ConcaveShape concaveShape;

//         concaveShape.setPointCount(12);
//         for (auto i = 0; i < poly->vertices.size(); ++i)
//         {
//             auto& v = poly->vertices[i];
//             concaveShape.setPoint(i, sf::Vector2f(v.x, v.y));

//         }

//         concaveShape.setFillColor(sf::Color::White);
//         concaveShape.setPosition(poly->rb->GetPosition().x, poly->rb->GetPosition().y);

//         window->draw(concaveShape);
//         window->draw(circle);


//         //window->draw(concaveShape);


//         // Display the window
//         window->display();

//     }


//     return 0;
// }