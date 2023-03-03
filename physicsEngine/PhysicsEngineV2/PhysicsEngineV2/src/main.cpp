#include <chrono>
#include <iostream>

#include "core/PhysicsWorld.h"
#include "entity/Entity.h"
#include "entity/EntityFactory.h"
#include "entity/PolygonEntity.h"
#include "entity/RectangleEntity.h"
#include "game/EventManager.h"
#include "SFML/Graphics.hpp"

void createHexagon(float radius, std::vector<FVector2>& hexagon)
{
	constexpr int vertexCount = 6;
	hexagon.resize(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
	{
		const float angle = i * 2.f * PI / (vertexCount+1); // Calculate the angle of the current vertex
		hexagon[i] = { radius * std::cos(angle), radius * std::sin(angle) };
	}
}


int main()
{
	const auto world = PhysicsWorld::GetWorld();

	constexpr int window_width = 800;
	constexpr int window_height = 600;

	const std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(sf::VideoMode(window_width, window_height), "Drawing Squares");

	auto rb1 = EntityFactory::create<RectangleEntity>(FVector2{ 700.f, 50.f }, FVector2{ 450, 450.f }, 10.f, 0.1f, true);

	auto rb2 = EntityFactory::create<RectangleEntity>(FVector2{ 300.f, 50.f }, FVector2{ 150.f, 250.f }, 10.f, 0.1f, true);
	rb2->rb->RotateTo(DegreesToRadians(40));

	EventManager eventManager = { window };

	std::vector<std::shared_ptr<PolygonEntity>> rbs;

	eventManager.addMousePressedCallback(sf::Mouse::Left, [&](const sf::Event& e)
		{
			float x = e.mouseButton.x;
			float y = e.mouseButton.y;

			std::vector<FVector2> hex_vertex;
			createHexagon(20.f, hex_vertex);

			const auto poly = EntityFactory::create<PolygonEntity>(hex_vertex, FVector2{ x, y }, 1.f, 1.f, false);
			rbs.push_back(poly);
		});

	eventManager.addKeyPressedCallback(sf::Keyboard::S, [&](const sf::Event& e)
		{
			rb1->rb->AddForce({ 0.f, 5.f });
		});

	eventManager.addKeyPressedCallback(sf::Keyboard::D, [&](const sf::Event& e)
		{
			rb1->rb->AddForce({ 5.f, 0.f });
		});


	auto time = std::chrono::high_resolution_clock::now();
	while (window->isOpen())
	{
		auto newTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float>(newTime - time).count();
		time = newTime;

		std::cout << "DT: " << dt << std::endl;

		// Handle events
		eventManager.processEvents();

		world->Step(dt, 1);

		if (!rbs.empty()) {
			for (int i = rbs.size() - 1; i >= 0; --i)
			{
				auto& ptr = rbs[i];
				if (ptr->rb->position.y > window_height + 100.f)
				{
					rbs.erase(std::ranges::find(rbs, ptr));
				}
			}
		}
		
		window->clear();

		sf::RectangleShape r1{ { rb1->rb->Width, rb1->rb->Height } };
		r1.setOrigin(r1.getSize() / 2.f);
		r1.setPosition({ rb1->rb->position.x, rb1->rb->position.y });
		r1.setRotation({ RadiansToDegrees(rb1->rb->angle) });

		window->draw(r1);

		sf::RectangleShape r2{ { rb2->rb->Width, rb2->rb->Height } };
		r2.setOrigin(r2.getSize() / 2.f);
		r2.setPosition({ rb2->rb->position.x, rb2->rb->position.y });
		r2.setRotation({ RadiansToDegrees(rb2->rb->angle) });

		window->draw(r2);

		for (auto& ent : rbs)
		{
			auto x = ent->rb->position.x;
			auto y = ent->rb->position.y;
			constexpr int vertexCount = 6;
			sf::VertexArray hexagon(sf::PrimitiveType::TrianglesFan, vertexCount + 2);
			hexagon[0].position = sf::Vector2f(x, y);
			hexagon[0].color = sf::Color::White;
			for (int i = 1; i <= vertexCount + 1; ++i)
			{
				float angle = i * 2.f * PI / vertexCount; // Calculate the angle of the current vertex
				angle += RadiansToDegrees(ent->rb->angle);
				hexagon[i].position = sf::Vector2f(x + 20.f * std::cos(angle), y + 20.f * std::sin(angle));
				hexagon[i].color = sf::Color::White;
			}


			window->draw(hexagon);

		}

		// Display the window
		window->display();

	}

	return 0;
}