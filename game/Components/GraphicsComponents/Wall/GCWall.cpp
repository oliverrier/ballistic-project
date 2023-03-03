#include "GCWall.h"

#include "game/GameObjects/Wall.h"


GCWall::GCWall()
{
}

void GCWall::renderImplementation(IGameObject& gameObject, sf::RenderWindow& window)
{
	Wall& wall = static_cast<Wall&>(gameObject);

	wall.m_shape.setOrigin(wall.m_shape.getSize() / 2.f);
	window.draw(wall.m_shape);
}
