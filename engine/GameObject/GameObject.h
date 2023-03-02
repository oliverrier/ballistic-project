#pragma once 

#include "../../Tools/DesignPatterns/Composite.h"
#include "../Components/InputComponent.h"
#include <SFML/Graphics.hpp>


class IScene;


class IGameObject: public Composite<IGameObject>
{
public:
	virtual ~IGameObject() {};

	virtual void processInput(sf::Event& inputEvent, IScene& scene) = 0;
	virtual void update(const float& deltaTime, IScene& scene) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
};

template<typename... MixinGameComponents>
class GameObject : public IGameObject, public MixinGameComponents...
{
public:
	void processInput(sf::Event& inputEvent, IScene& scene) override
	{
		this->processInputImplementation(*this, inputEvent, scene);
	}

	void update(const float& deltaTime, IScene& scene) override
	{
		this->updateImplementation(deltaTime, *this, scene);
	}

	void render(sf::RenderWindow& window) override
	{
		this->renderImplementation(*this, window);
	}

};
