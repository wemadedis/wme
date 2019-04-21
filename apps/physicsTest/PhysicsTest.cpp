#include <rte/RTE.hpp>

#include <iostream>

void ConfigureGame(RTE::RTEConfig &config)
{
    std::cout << "Configuring" << std::endl;
    config.WindowConfig.ApplicationName = "Banana";
}

void OnGameStart(RTE::Runtime::SceneManager &sceneManager)
{
    sceneManager.MakeScene();
    std::cout << "Called" << std::endl;
}