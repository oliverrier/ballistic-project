#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <engine/utils/Math/Vector2.h>
#include <iostream>
#include <string>
#include "../../core/Entity/Entity.h"

template<typename T>
struct HudEntityFixed : public sf::RectangleShape {

public:
    HudEntityFixed(const FVector2& relativePosition, const FVector2& size, const sf::Color& color, std::shared_ptr<RigidBody> entity_rb) : m_entity_rb(entity_rb)
    {
        m_initialPosition = relativePosition;
        m_relativePosition = relativePosition;

        //setPosition(relativePosition);
        setSize(sf::Vector2f(size.x, size.y));
        setFillColor(color);

        hudVector.push_back(*this);
    }

    ~HudEntityFixed() = default;

    void draw(sf::RenderTarget& renderTarget, sf::RenderStates state) const override {
        renderTarget.draw(static_cast<sf::RectangleShape>(*this), state);
    }

    void setRelativePosition(const FVector2 position) {
        m_relativePosition = position;
    }

    void correctPosition() {
        sf::RectangleShape::setPosition(m_entity_rb->position.x + m_relativePosition.x, m_entity_rb->position.y - m_relativePosition.y);

    }

    FVector2 getRelativePosition() {
        return m_relativePosition;
    }


    FVector2 getInitialPosition() {
        return m_initialPosition;
    }

public:
    static std::vector<HudEntityFixed<T>> hudVector;
    std::shared_ptr<RigidBody> m_entity_rb;


private:

    FVector2 m_relativePosition;
    FVector2 m_initialPosition;
};

//vector initialization
template<typename T>
std::vector<HudEntityFixed<T>> HudEntityFixed<T>::hudVector;
