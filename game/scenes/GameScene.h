#pragma once

#include "engine/Scene/Scene.h"
#include "engine/Ui/Buttons/RectangleButton.h"
#include "game/Camera.h"
#include "game/EventManager.h"
#include "engine/Ui/HUD/HudElement.h"
#include "game/GameObjects/Ground.h"
#include "game/GameObjects/Bullet.h"
#include "game/GameObjects/Character/Character.h"
#include "engine/Ui/HUD/HudArrow.h"
#include "game/GameObjects/Character/Character.h"
#include "engine/Ui/HUD/HudEntityFixed.h"


class GameScene : public IScene
{
public:
	GameScene();
	~GameScene() = default;

	void processInput(sf::Event& inputEvent) override;
	void update(const float& deltaTime) override;
	void render() override;

	void initButtons();
	void NextPlayer();
	void initObjects();
	void registerEvents();

	float shootingAngle;

	float windAngle;
	float windForce;

	bool canShoot = true;
	float time;
	bool is_player_1_turn = true;
	int player_index_to_play = 0;

private:

	//PhysicsWorld* m_physicsWorld;
	EventManager* m_eventManager;
	Camera* m_camera;

	std::shared_ptr<RectangleButton> startButton;
	std::shared_ptr<RectangleButton> exitButton;

	sf::Sprite* m_backgroundSprite;
		
	std::shared_ptr<Ground> m_platform;
	std::shared_ptr<Character> m_currentCharacter;

	std::shared_ptr<Character> player1;
	std::shared_ptr<Character> player2;

	std::shared_ptr<World> m_world;

	// Interface elements
	std::shared_ptr<HudElement<std::string>> pannel;
	std::shared_ptr<HudElement<std::string>> moveInfo;
	std::shared_ptr<HudElement<std::string>> aimInfo;
	std::shared_ptr<HudElement<std::string>> shootInfo;
	std::shared_ptr<HudElement<std::string>> timer;
	std::shared_ptr<HudElement<std::string>> playerInfo;

	std::shared_ptr<HudArrow> windArrow;
	std::shared_ptr<HudEntityFixed<float>> lifeBar1;
	std::shared_ptr<HudEntityFixed<float>> lifeBar2;

	std::shared_ptr<Character> character;

	std::vector < std::shared_ptr<HudElement<std::string>> > hudElements;

public:
	std::vector<std::shared_ptr<Bullet>> m_fragmentation;

};

