#include "game.h"
#include "operators.h"

Game::Game(Display *display) : display_(display), is_new_highscore_(false) {}

Game::~Game() {}

void Game::onButtonUpChange(bool state) {}

void Game::onButtonDownChange(bool state) {}

void Game::onButtonRightChange(bool state) {}

void Game::onButtonLeftChange(bool state) {}

extern "C" void __cxa_pure_virtual()
{
	while (1)
		;
}
