# We Made Dis Engine - WME

This repository hosts the game engine produced during an elective project at in Spring 2019.

The engine is produced by three students from the Games line tech track:Adrian Radziszewski (adra), Daniel Gaard Hansen (danh), and Roman Karaba (roka).

The engine has these features:

 - NVIDIA RTX ray tracing via Vulkan
 - Physics simulation via BulletPhysics
 - Spatial sound via SFML
 - Input via GLFW
 - GameObject-Component based runtime architecture

## How do I build it?

The engine uses CMake to make a build system.
The build works only for 64-bit builds, without touching any CMake settings, and we make no guarantees that it will of you do.

 1. Build the *rte* target
 2. Link with *rte* with `target_link_libraries{<My_Game> PUBLIC rte}`

## How do I make a game in it?

 1. Link with the *rte* CMake target
 2. Define these functions:

```cpp
void ConfigureGame(RTE::RTEConfig &config){}
```
Customize the engine settings, insert (relative) paths to models and textures, toggle raytracing on/off using this config parameter

```cpp
void OnGameStart(RTE::Runtime::SceneManager &sceneManager){}
```

Get a scene from the manager with `sceneManager.GetActiveScene()` and use it to define a scene.

Scenes are defined using *GameObjects* and *Components*. GameObject refers to an in-game entity moving around, acting as camera, lighting a scene, or whatever you want it to do. These GameObjects are primarily just containers for components, which holds the actual functionality.

Components need to be defined before they can be added to a game object. This is done via the method call 
```cpp
ComponentPoolId myCompPoolId = mySceneInstancePointer->DefineComponent<MyComponentType, NumberOfComponents>();
```
This will create a new object pool of *MyComponentType* components.

These can now be added to game objects. So let's get a game object, and add a component to it.

```cpp
Scene *myScene = sceneManager.GetActiveScene();
ComponentPoolId myCompPoolId = myScene->DefineComponent<MyComponentType, NumberOfComponents>();
GameObject *myGameObject = myScene->CreateGameObject();
MyComponentType *myCompInstance = myScene->AddComponent<MyComponentType>(myCompPoolId, myGameObject);
```

After OnGameStart has finished executing, myCompInstance's Update function will now be called every frame. 

RTE defines 8 standard components, all stored in the `RTE::StdComponents` namespace. These components must have a default constructor.
All components (probably including those you will write yourself) implement an `Initialize(...)` function as a replacement for the constructor.

The following table provides an overview of the 8 standard components available for use.

Component name           | Initialization requirement                                       | Use 
-------------------------|------------------------------------------------------------------| ----------
TransformComponent       | None                                                             | Contains position, rotation, and scale for game objects
MeshComponent            | Transform, path to model, (path to texture = none)               | Renders the model and texture on the screen according to the transform.
CameraComponent          |Transform                                                         | Films the world according to the transform
PointLightComponent      | Transform, color, radius                                         | Lights the scene from the transform's position, with the passed color radius
DirectionalLightComponent|Transform, color                                                  | Lights the scene from the direction of the transforms rotation with the color
PhysicsComponent         | Transform, mass, List of colliders                               | Lets this gameobject interact with the physics system by adding a rigidbody
AudioComponent           | Path to sound file, transform, (volume = 100), (soundOffset = 0) | Plays sounds
ListenerComponent        | Transform, enabled                                               | Listens for sounds, only one listener can be active at one time

The [Pong](apps/Pong "A simple pong game in RTE") example gives an overview of how custom components are made, used, and interact with the rest of the engine.

## How do I contribute?

Thank you for showing interest, but for now this project is for our code only.
