#include <rte/RTE.hpp>

#include <iostream>

void ConfigureGame(RTEConfig &config)
{
    std::cout << "Configuring" << std::endl;
    config.WindowName = "Banana";
}

void OnGameStart(RTE::Runtime::SceneManager &sceneManager)
{
    sceneManager.MakeScene();
    std::cout << "Called" << std::endl;
}