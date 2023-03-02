#include "GCGround.h"
#include "game/GameObjects/Ground.h"

void GCGround::renderImplementation(IGameObject& gameObject, sf::RenderWindow& window) {
	const Ground& ground = reinterpret_cast<Ground&>(gameObject);
	window.draw(ground.m_shape);
}