#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <engine/utils/Math/Vector2.h>
#include <iostream>

#pragma once

struct RectangleButton : public sf::RectangleShape
{
public:
    RectangleButton(const FVector2& position, const FVector2& size, const sf::Color& color, const sf::String text)
    {
        m_initialPosition = position;

        setPosition(position);
        setSize(sf::Vector2f(size.x, size.y));
        setFillColor(color);

        m_text.setString(text);

        if (!m_font.loadFromFile("Assets/Game.ttf")) {
            std::cout << "can't load font" << std::endl;
        };

        m_text.setFont(m_font);
        m_text.setCharacterSize(24);
        m_text.setFillColor(sf::Color::White);

        //rectButtonVect.push_back(this);
    }

    ~RectangleButton() = default;

    void setOnClick(std::function<void()> onClick) {
        m_onClick = onClick;
    }

    void handleClick(const FVector2& mousePosition) {
        if (getLocalBounds().contains(mousePosition.x - m_initialPosition.x, mousePosition.y - m_initialPosition.y)) {
            if (m_onClick != nullptr) {
                m_onClick();
            }
        }
    }

    void draw(sf::RenderTarget& renderTarget, sf::RenderStates state) const override {
        renderTarget.draw(static_cast<sf::RectangleShape>(*this), state);
        renderTarget.draw(m_text, state);

    }

    void setPosition(const FVector2 position){
        sf::RectangleShape::setPosition(position.x, position.y);

        // Place the text in the center
        sf::FloatRect textRect = m_text.getLocalBounds();
        m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        m_text.setPosition(position.x + getSize().x / 2.0f, position.y + getSize().y / 2.0f);

    }

    FVector2 getInitialPosition() {
        return m_initialPosition;
    }

public:
    //static std::vector<RectangleButton> rectButtonVect;

private:
    std::function<void()> m_onClick;

    sf::Text m_text;
    sf::Font m_font;

    FVector2 m_initialPosition;

};

//std::vector<RectangleButton*> RectangleButton::rectButtonVect;