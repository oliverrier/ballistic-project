#ifndef PHYSICSSCENE_H
#define PHYSICSSCENE_H

#include "engine/Scene/Scene.h"
#include "game/GameObjects/UI/Button.h"
#include "engine/core/PhysicsWorld.h"
#include "engine/core/Entity/CircleEntity.h"
#include "engine/core/Entity/SquareEntity.h"

#include "game/Camera.h"
#include "game/EventManager.h"

class PhysicsScene : public IScene
{
public:
	PhysicsScene();
	~PhysicsScene();

	void processInput(sf::Event& inputEvent) override;
	void update(const float& deltaTime) override;
	void render() override;

	void initButtons();
	void initObjects();
	void registerEvents();

	std::shared_ptr<PhysicsWorld> m_world;

	EventManager m_eventManager;
	Camera m_camera;

	std::shared_ptr<SquareEntity> m_square2;
	std::shared_ptr<CircleEntity> m_c;
	std::shared_ptr<SquareEntity> m_sol;
};

#endif // !PHYSICSSCENE_H
