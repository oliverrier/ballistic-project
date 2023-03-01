#ifndef STARTSCENE_H
#define STARTSCENE_H

#include "engine/Scene/Scene.h"
#include "game/GameObjects/UI/Button.h"

class StartScene : public IScene
{
public:
	StartScene();
	~StartScene();

	void processInput(sf::Event& inputEvent) override;
	void update(const float& deltaTime) override;
	void render() override;

	void initButtons();

};

#endif // !STARTSCENE_H
