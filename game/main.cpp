#include <iostream>
#include <chrono>
#include <engine/Game/Game.h>

#include "Scenes/StartScene.h"
#include "Scenes/SceneEnum.h"


int main()
{
    Game* game = Game::GetInstance();
    game->addScenes(new StartScene());
    game->setCurrentScene(ScenesEnum::START_SCENE);

    game->run(sf::VideoMode(1920, 1080), "ballistic-project", sf::Style::Fullscreen);

    return 0;
}