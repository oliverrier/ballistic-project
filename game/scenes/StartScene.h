#pragma once

#include "engine/Scene/Scene.h"
#include "game/GameObjects/UI/Button.h"
#include "engine/Ui/Buttons/RectangleButton.h"

class StartScene : public IScene
{
public:
	StartScene();
	~StartScene(); 

	void processInput(sf::Event& inputEvent) override;
	void update(const float& deltaTime) override;
	void render() override;
	void initButtons();

private:

	std::shared_ptr<RectangleButton> startButton;
	std::shared_ptr<RectangleButton> exitButton;

	sf::Sprite* m_backgroundSprite;

};
