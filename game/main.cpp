#include <iostream>
#include <chrono>
#include <engine/Game/Game.h>

#include "Scenes/StartScene.h"
#include "scenes/GameScene.h"
#include "Scenes/SceneEnum.h"


int main()
{
    Game* game = Game::GetInstance();
    game->addScenes(new StartScene());
    game->addScenes(new GameScene());

    game->setCurrentScene(ScenesEnum::GAME_SCENE);

    game->run(sf::VideoMode(1920, 1080), "ballistic-project", sf::Style::Resize);

    return 0;
}

