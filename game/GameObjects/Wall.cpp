#include "Wall.h"

Wall::Wall(Vec2& size, Vec2& position) {
    m_body = EntityFactory::create<RectEntity>(size, position, staticBody);

    m_shape = sf::RectangleShape{ {size.x, size.y } };
    m_shape.setPosition({ position.x, position.y });
}