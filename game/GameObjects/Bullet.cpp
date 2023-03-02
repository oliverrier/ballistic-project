#include "Bullet.h"

#include <iostream>

Bullet::Bullet(std::shared_ptr<CircleEntity> body, sf::CircleShape* circle, float angle) : m_body(body), m_circle(circle), angle(angle){
	std::cout << "Bullet crée" << std::endl;
}