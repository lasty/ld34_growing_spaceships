
#pragma once

// Stores global constants, and a select few global variables

#include <string>

//#include <SDL_render.h>

extern std::string DATA_PATH;

struct SDL_Renderer;

extern SDL_Renderer * RENDERER;


extern class Game * GAME;

extern class Assets * ASSETS;


enum class Mode
{
	Scavenge, Combat
};

