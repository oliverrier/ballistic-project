#include "GCBullet.h"
#include "game/GameObjects/Bullet.h"

GCBullet::GCBullet() {

}

void GCBullet::renderImplementation(IGameObject& gameObject, sf::RenderWindow& window) {

	Bullet& bullet = reinterpret_cast<Bullet&>(gameObject);
	window.draw(*bullet.m_circle);

}