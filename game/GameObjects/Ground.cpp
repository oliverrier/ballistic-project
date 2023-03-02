#include "Ground.h"

#include <iostream>

Ground::Ground(std::vector<Vec2>& vertices, Vec2& position) {
	m_body = EntityFactory::create<PolygonEntity>(vertices, position);
	thor::ConcaveShape concaveShape;

	concaveShape.setPointCount(vertices.size());
    for (auto i = 0; i < vertices.size(); ++i)
    {
        auto& v = vertices[i];
        concaveShape.setPoint(i, sf::Vector2f(v.x, v.y));
    }

    concaveShape.setFillColor(sf::Color::White);
    concaveShape.setPosition(position.x, position.y);

    m_shape = concaveShape;

	std::cout << "Ground créé" << std::endl;
}