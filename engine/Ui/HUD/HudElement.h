#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <engine/utils/Math/Vector2.h>
#include <iostream>
#include <string>

template<typename T>
struct HudElement : public sf::RectangleShape {

public:
    HudElement(const FVector2& position, const FVector2& size, const sf::Color& color, T* textValue, const std::string& texturePath = "") : m_text(textValue)
    {
        m_initialPosition = position;

        setPosition(position);
        setSize(sf::Vector2f(size.x, size.y));
        setFillColor(color);


        m_textDisplayed.setString(std::to_string(*m_text));

        if (!m_font.loadFromFile("Assets/Game.ttf")) {
            std::cout << "can't load font" << std::endl;
        };

        m_textDisplayed.setFont(m_font);
        m_textDisplayed.setCharacterSize(24);
        m_textDisplayed.setFillColor(sf::Color::White);

        if (!texturePath.empty()) {
            if (!m_texture.loadFromFile(texturePath)) {
                std::cout << "can't load texture" << std::endl;
            }
            m_sprite.setTexture(m_texture);
            m_sprite.setPosition(position.x, position.y);
            m_sprite.setScale(size.x / m_texture.getSize().x, size.y / m_texture.getSize().y);
        }

        //hudVector.push_back(*this);
    }

    ~HudElement() = default;

    void draw(sf::RenderTarget& renderTarget, sf::RenderStates state) const override {
        renderTarget.draw(static_cast<sf::RectangleShape>(*this), state);

        if (m_sprite.getTexture() != nullptr) {
            renderTarget.draw(m_sprite, state);
        }

        renderTarget.draw(m_textDisplayed, state);

    }


    void setPosition(const FVector2 position) {
        sf::RectangleShape::setPosition(position.x, position.y);
        m_textDisplayed.setString(std::to_string(*m_text));

        if (m_sprite.getTexture() != nullptr) {
            m_sprite.setPosition(position.x, position.y);
        }
        
        // Place the text in the center
        sf::FloatRect textRect = m_textDisplayed.getLocalBounds();
        m_textDisplayed.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        m_textDisplayed.setPosition(position.x + getSize().x / 2.0f, position.y + getSize().y / 2.0f);

    }

    FVector2 getInitialPosition() {
        return m_initialPosition;
    }

public:
    //static std::vector<HudElement<T>> hudVector;

private:

    T* m_text;
    sf::Text m_textDisplayed;
    sf::Font m_font;

    sf::Texture m_texture;
    sf::Sprite m_sprite;

    FVector2 m_initialPosition;
};

//vector initialization
//template<typename T>
//std::vector<HudElement<T>> HudElement<T>::hudVector;




//String specialisation
template<>
struct HudElement<std::string> : public sf::RectangleShape {

public:
    HudElement(const FVector2& position, const FVector2& size, const sf::Color& color = sf::Color::Transparent, std::string textValue = "", const std::string& texturePath = "") : m_text(textValue)
    {
        m_initialPosition = position;

        setPosition(position);
        setSize(sf::Vector2f(size.x, size.y));
        setFillColor(color);

        m_textDisplayed.setString(m_text);

        if (!m_font.loadFromFile("Assets/Game.ttf")) {
            std::cout << "can't load font" << std::endl;
        };

        m_textDisplayed.setFont(m_font);
        m_textDisplayed.setCharacterSize(24);
        m_textDisplayed.setFillColor(sf::Color::White);

        if (!texturePath.empty()) {
            if (!m_texture.loadFromFile(texturePath)) {
                std::cout << "can't load texture" << std::endl;
            }
            m_sprite.setTexture(m_texture);
            m_sprite.setPosition(position.x, position.y);
            m_sprite.setScale(size.x / m_texture.getSize().x, size.y / m_texture.getSize().y);
        }

        //hudVector.push_back(*this);
    }

    ~HudElement() = default;

    void draw(sf::RenderTarget& renderTarget, sf::RenderStates state) const override {
        renderTarget.draw(static_cast<sf::RectangleShape>(*this), state);

        if (m_sprite.getTexture() != nullptr) {
            renderTarget.draw(m_sprite, state);
        }

        renderTarget.draw(m_textDisplayed, state);
    }


    void setPosition(const FVector2 position) {
        sf::RectangleShape::setPosition(position.x, position.y);

        if (m_sprite.getTexture() != nullptr) {
            m_sprite.setPosition(position.x, position.y);
        }
        // Place the text in the center
        sf::FloatRect textRect = m_textDisplayed.getLocalBounds();
        m_textDisplayed.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        m_textDisplayed.setPosition(position.x + getSize().x / 2.0f, position.y + getSize().y / 2.0f);

    }

    void setSpriteRotation(float angle) {
        if (m_sprite.getTexture() != nullptr) {
            m_sprite.setRotation(angle);
        }
    }

    FVector2 getInitialPosition() {
        return m_initialPosition;
    }

public:
    //static std::vector <HudElement<std::string>> hudVector;

private:

    std::string m_text;
    sf::Text m_textDisplayed;
    sf::Font m_font;

    sf::Texture m_texture;
    sf::Sprite m_sprite;

    FVector2 m_initialPosition;
};

//vector initialization
//std::vector<HudElement<std::string>> HudElement<std::string>::hudVector;