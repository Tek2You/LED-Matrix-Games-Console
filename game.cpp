#include "game.h"
#include "operators.h"


Game::Game(Display *display) : display_(display), is_new_highscore_(false)
{

}

Game::~Game()
{

}

extern "C" void __cxa_pure_virtual() { while (1); }
