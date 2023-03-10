#include "Scene.h"

#include "../Game/Game.h"
#include "../GameObject/GameObject.h"

IScene::IScene() : m_window(Game::GetInstance()->getWindow())
{
}

IScene::~IScene()
{
	clearGameObjects();
}


const sf::Vector2i IScene::getMousePositionScreen()
{
	return sf::Mouse::getPosition();
}

const sf::Vector2i IScene::getMousePositionWindow()
{
	return sf::Mouse::getPosition(*m_window);
}

const sf::Vector2f IScene::getMousePositionView()
{
	return m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
}

void IScene::processInput(sf::Event& inputEvent)
{
	for (const auto& pGameObject : m_gameObjects)
	{
		pGameObject->processInput(inputEvent, *this);
	}
}

void IScene::update(const float& deltaTime)
{
	for (const auto& pGameObject : m_gameObjects)
	{
		if (pGameObject)
			pGameObject->update(deltaTime, *this);

	}
}

void IScene::render()
{
	for (const auto& pGameObject : m_gameObjects)
	{
		pGameObject->render(*m_window);
	}
}


IGameObject* IScene::getGameObject(const size_t index)
{
	return m_gameObjects.at(index).get();
}

std::vector<std::shared_ptr<IGameObject>>& IScene::getGameObjects()
{
	return m_gameObjects;
}

void IScene::clearGameObjects()
{
	m_gameObjects.clear();
}

void IScene::RemoveGameObject(IGameObject* game_object)
{
	for (int i = m_gameObjects.size() - 1; i >= 0; --i)
	{
		if (m_gameObjects[i].get() == game_object)
		{
			m_gameObjects.erase(m_gameObjects.begin() + i);
		}
	}
}
