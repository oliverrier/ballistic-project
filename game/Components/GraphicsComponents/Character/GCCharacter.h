#ifndef GCCHARACTER_H
#define GCCHARACTER_H

#include "../../../../Engine/Components/GraphicsComponent.h"

class IGameObject;


struct GCCharacter : IGraphicsComponent
{
	GCCharacter();
	virtual void renderImplementation(IGameObject& gameObject, sf::RenderWindow& window) override;
};

#endif // GCCHARACTER_H