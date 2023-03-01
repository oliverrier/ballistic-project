#include <SFML/Graphics.hpp>

#pragma once

struct CircleButton : public sf::CircleShape
{
public:
    CircleButton(const float radius, std::size_t pointCount, const sf::Color& color, const sf::Vector2f& position)
    {
        setRadius(radius);
        setPointCount(pointCount);
        setFillColor(color);
        setPosition(position);

    }
};