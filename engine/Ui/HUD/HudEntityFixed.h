#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <engine/utils/Math/Vector2.h>
#include <iostream>
#include <string>

#include "game/GameObjects/Character/Character.h"

template<typename T>
struct HudEntityFixed : public sf::RectangleShape {

public:
    HudEntityFixed(const FVector2& relativePosition, const FVector2& size, const sf::Color& color, std::shared_ptr<Character> character) : m_character(character), m_initialeSize(size)
    {
        m_initialPosition = relativePosition;
        m_relativePosition = relativePosition;

        setSize(sf::Vector2f(size.x, size.y));
        setFillColor(color);

    }

    ~HudEntityFixed() = default;

    void draw(sf::RenderTarget& renderTarget, sf::RenderStates state) const override {
        renderTarget.draw(static_cast<sf::RectangleShape>(*this), state);
    }

    void setRelativePosition(const FVector2 position) {
        m_relativePosition = position;
    }

    void correctSize() {
        //std::cout << m_initialeSize.x * (m_character->getHealth() / m_character->getHealth()) << std::endl;
        sf::RectangleShape::setSize(sf::Vector2f(m_initialeSize.x * (m_character->getHealth() / m_character->getMaxHealth()), m_initialeSize.y));
    }

    void correctPosition() {
        sf::RectangleShape::setPosition(m_character->m_body->rb->GetPosition().x + m_relativePosition.x, m_character->m_body->rb->GetPosition().y - m_relativePosition.y);
    }

    FVector2 getRelativePosition() {
        return m_relativePosition;
    }


    FVector2 getInitialPosition() {
        return m_initialPosition;
    }

public:

    std::shared_ptr<Character> m_character;

private:
    FVector2 m_initialeSize;
    FVector2 m_relativePosition;
    FVector2 m_initialPosition;
};
