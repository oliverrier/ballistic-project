#ifndef PCCHARACTER_H
#define PCCHARACTER_H

#include "../../../../Engine/Components/PhysicsComponent.h"

class IGameObject;


struct PCCharacter : IPhysicsComponent
{
	PCCharacter();
	void updateImplementation(const float& deltaTime, IGameObject& gameObject, IScene& scene) override;

private:
	bool m_callbackIsCalled;
};

#endif // PCCHARACTER_H