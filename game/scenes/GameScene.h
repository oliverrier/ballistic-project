#pragma once

#include "engine/Scene/Scene.h"
#include "engine/Ui/Buttons/RectangleButton.h"
#include "game/Camera.h"
#include "game/EventManager.h"
#include "engine/Ui/HUD/HudElement.h"
#include "game/GameObjects/Ground.h"
#include "game/GameObjects/Bullet.h"
#include "engine/Ui/HUD/HudArrow.h"


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

	float shootingAngle;

	float windAngle;
	float windForce;

	bool canShoot = true;
	float time;
	bool is_player_1_turn = true;

private:

	//PhysicsWorld* m_physicsWorld;
	EventManager* m_eventManager;
	Camera* m_camera;

	std::shared_ptr<RectangleButton> startButton;
	std::shared_ptr<RectangleButton> exitButton;

	sf::Sprite* m_backgroundSprite;
		
	std::shared_ptr<Ground> m_platform;

	std::shared_ptr<World> m_world;

	// Interface elements
	std::shared_ptr<HudElement<std::string>> pannel;
	std::shared_ptr<HudElement<std::string>> moveInfo;
	std::shared_ptr<HudElement<std::string>> aimInfo;
	std::shared_ptr<HudElement<std::string>> shootInfo;
	std::shared_ptr<HudElement<std::string>> timer;

	std::shared_ptr<HudArrow> windArrow;
	std::vector < std::shared_ptr<HudElement<std::string>> > hudElements;

};

