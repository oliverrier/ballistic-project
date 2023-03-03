#pragma once

#include "../../../../Engine/Components/GraphicsComponent.h"

class IGameObject;


struct GCWall : IGraphicsComponent
{
	GCWall();
	virtual void renderImplementation(IGameObject& gameObject, sf::RenderWindow& window) override;
};

