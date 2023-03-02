#include "GCGround.h"
#include "game/GameObjects/Ground.h"

void GCGround::renderImplementation(IGameObject& gameObject, sf::RenderWindow& window) {
	const Ground& ground = static_cast<Ground&>(gameObject);

	auto shape = ground.m_shape;

	window.draw(ground.m_shape);
}