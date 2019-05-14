# Ray Tracing Game Engine - RTE

This repository hosts the game engine produced during an elective project at in Spring 2019.

The engine is produced by three students from the Games line tech track:Adrian Radziszewski (adra), Daniel Gaard Hansen (danh), and Roman Karaba (roka).

The engine has these features:

 - NVIDIA RTX ray tracing via Vulkan
 - Physics simulation via BulletPhysics
 - Spatial sound via SFML
 - Input via GLFW
 - GameObject-Component based runtime architecture
 
## How do I build it?

## How do I make a game in it?

 1. Link with the CMake **rte** target
 2. Define these two functions:

```cpp
void ConfigureGame(RTE::RTEConfig &config){}
void OnGameStart(RTE::Runtime::SceneManager &sceneManager){}
```

## How do I contribute?

Thank you for showing interest, but for now this project is for our code only.
