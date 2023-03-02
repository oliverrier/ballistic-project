#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "engine/Scene/Scene.h"
#include "game/GameObjects/UI/Button.h"
#include "engine/Ui/Buttons/RectangleButton.h"
#include "game/Camera.h"
#include "game/EventManager.h"
#include "engine/Entity/RectEntity.h"
#include "engine/Ui/HUD/HudElement.h"


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
		
	std::shared_ptr<RectEntity> m_platform;
	sf::RectangleShape* floor;

	std::shared_ptr<CircleEntity> body;
	sf::CircleShape* m_circle;

	std::shared_ptr<World> m_world;

	std::shared_ptr<HudElement<std::string>> pannel;
	std::shared_ptr<HudElement<std::string>> moveInfo;
	std::shared_ptr<HudElement<std::string>> aimInfo;
	std::shared_ptr<HudElement<std::string>> shootInfo;

	std::vector < std::shared_ptr<HudElement<std::string>> > hudElements;

};

#endif // !GAMESCENE_H
