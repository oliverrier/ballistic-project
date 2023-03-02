#pragma once

#include "engine/Components/GraphicsComponent.h"

class IGameObject;

struct GCGround : IGraphicsComponent
{
	void renderImplementation(IGameObject& gameObject, sf::RenderWindow& window) override;
};