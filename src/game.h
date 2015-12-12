
#pragma once


#include <SDL_events.h>


class Game
{
public:
	Game();

private:
	bool running = true;


public:
	bool GetRunning() const { return running; }


	void Update(float dt);

	void OnInput(SDL_Event &event);


	void Render();

	void ResizeWindow(int w, int h);

};


