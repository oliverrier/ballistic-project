#pragma once

#include "engine/Components/GraphicsComponent.h"


class IGameObject;

struct GCBullet : public IGraphicsComponent
{
	GCBullet();
	void renderImplementation(IGameObject& gameObject, sf::RenderWindow& window) override;
};