#pragma once


#include <SFML/Graphics.hpp>


class IGameObject;

class IScene 
{
public:
    IScene();
    virtual ~IScene();

    virtual const sf::Vector2i getMousePositionScreen();
    virtual const sf::Vector2i getMousePositionWindow();
    virtual const sf::Vector2f getMousePositionView();

    virtual void processInput(sf::Event& inputEvent);
    virtual void update(const float& deltaTime);
    virtual void render();

    template <typename... Args>
    void addGameObjects(Args... gameObjects);

    IGameObject* getGameObject(const size_t index);

    std::vector<IGameObject*>& getGameObjects();

    void clearGameObjects();

protected:
    std::shared_ptr<sf::RenderWindow> m_window;
    std::vector<IGameObject*> m_gameObjects;
};


template<typename ...Args>
inline void IScene::addGameObjects(Args ...gameObjects)
{
    (m_gameObjects.push_back(gameObjects), ...);
}
