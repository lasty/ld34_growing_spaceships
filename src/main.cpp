
// Main startup file
// Initialize stuff and has main game loop for SDL2



#include "globals.h"


#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#ifdef __WIN32__
	#undef main
#endif


#include <fstream>
#include <memory>
#include <iostream>
#include <assert.h>


#include "game.h"
#include "assets.h"


//define storage for globals
std::string DATA_PATH;

SDL_Renderer * RENDERER = nullptr;
Game * GAME = nullptr;
Assets * ASSETS = nullptr;



void FindDataDirectory()
{
	for (auto path : {"", "../", "../../"} )
	{
		std::string path_dir = std::string(path) + "data/";
		std::string try_path = path_dir + "data_directory.txt";

		std::ifstream in{try_path.c_str()};
		if (in)
		{
			DATA_PATH = path_dir;
			std::cout << "Data Path found: " << DATA_PATH << std::endl;
			return;
		}
	}

	throw std::runtime_error("Could not locate the data directory");
}


void InitGlobals()
{
	FindDataDirectory();

	//Renderer deferred until game loop

	//Game set in Game constructor
	//Assets set in Assets constructor
}


void InitSDL2()
{
	auto sdl_init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (sdl_init < 0)
	{
		throw std::runtime_error("SDL Call failed");
	}

	auto ttf_init = TTF_Init();
	if (ttf_init < 0)
	{
		throw std::runtime_error("SDL TTF init  failed");
	}

	auto image_init = IMG_Init(0);
	if (image_init < 0)
	{
		throw std::runtime_error("SDL IMG  init  failed");
	}

	auto mixer_init = Mix_Init(0);
	if (mixer_init < 0)
	{
		throw std::runtime_error("SDL Mixer  init  failed");
	}

	auto audio_init = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	if (audio_init < 0)
	{
		throw std::runtime_error("SDL Mixer OpenAudio failed");
	}
	Mix_AllocateChannels(64);
}


void ShutdownSDL2()
{
	Mix_CloseAudio();

	SDL_Quit();
}


void CreateRenderer(SDL_Window *window)
{
	assert(RENDERER == nullptr);

	auto flags = SDL_RENDERER_ACCELERATED;
	//auto flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

	RENDERER = SDL_CreateRenderer(window, -1, flags);

	if(not RENDERER)
	{
		throw std::runtime_error("Could not create SDL Renderer");
	}

	SDL_SetRenderDrawBlendMode(RENDERER, SDL_BLENDMODE_BLEND);
}


void DestroyRenderer()
{
	SDL_DestroyRenderer(RENDERER);
	RENDERER = nullptr;
}

void RunGameLoop()
{
	auto win_flags = SDL_WINDOW_RESIZABLE;

	auto window = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>
	{SDL_CreateWindow("LD33 Monster", 50, 50, 1200, 800, win_flags), SDL_DestroyWindow};

	CreateRenderer(window.get());

	assert(ASSETS == nullptr);
	Assets assets;
	assert(ASSETS == &assets);

	assert(GAME == nullptr);
	Game game;
	assert(GAME == &game);
	game.ResizeWindow(1200, 800);


	bool running = true;
	Uint32 last_time = SDL_GetTicks();

	Uint32 fps_timer = SDL_GetTicks();
	int fps_frames = 0;

	while(running and game.GetRunning())
	{

		//Process event loop
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) { running = false;  break; }
					//fallthrough
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEWHEEL:
				case SDL_KEYUP:
					game.OnInput(event);
					break;

				case SDL_QUIT:
					running = false;
					break;
			}
		}


		// Update

		Uint32 this_time = SDL_GetTicks();

		float dt = (this_time - last_time) / 1000.0f;
		last_time = this_time;

		//const float FPS = 30;
		//float dt = 1.0f / FPS;

		fps_frames++;
		if (this_time > fps_timer + 1000)
		{

			std::cout << "FPS: " << fps_frames << std::endl;

			fps_frames = 0;
			fps_timer = this_time;
		}


		game.Update(dt);


		// Render

		game.Render();


		SDL_Delay(1);  //To prevent too high FPS

	}

	DestroyRenderer();
}


int main()
{
	InitGlobals();


	InitSDL2();


	RunGameLoop();


	ShutdownSDL2();
}

