#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "engine/Scene/Scene.h"
#include "game/GameObjects/UI/Button.h"
#include "engine/Ui/Buttons/RectangleButton.h"
#include "game/Camera.h"
#include "game/EventManager.h"


class GameScene : public IScene
{
public:
	GameScene();
	~GameScene() = default;

	void processInput(sf::Event& inputEvent) override;
	void update(const float& deltaTime) override;
	void render() override;

	void initButtons();
	void initObjects();
	void registerEvents();

private:

	//PhysicsWorld* m_physicsWorld;
	EventManager* m_eventManager;
	Camera* m_camera;

	std::shared_ptr<RectangleButton> startButton;
	std::shared_ptr<RectangleButton> exitButton;

	sf::Sprite* m_backgroundSprite;

};

#endif // !GAMESCENE_H