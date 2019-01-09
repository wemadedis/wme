#include "Game.h"
#include "Engine.h"

Game::Game()
{
	Engine* e = new Engine();
	e->sayHello();
}


Game::~Game()
{
}
