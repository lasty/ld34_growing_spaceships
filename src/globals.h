
#pragma once

// Stores global constants, and a select few global variables

#include <string>

//#include <SDL_render.h>

extern std::string DATA_PATH;

struct SDL_Renderer;

extern SDL_Renderer * RENDERER;


extern class Game * GAME;

extern class Assets * ASSETS;


constexpr float TRACTOR_SPEED = 0.3f;

constexpr float MASTER_VOLUME = 0.4f;

enum class Mode
{
	Scavenge, Combat, GameOver
};

