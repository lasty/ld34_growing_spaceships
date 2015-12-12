
#pragma once


#include <SDL_events.h>

#include "camera.h"
#include "ship.h"


class Game
{
public:
	Game();

private:
	bool running = true;

	Camera world_cam;
	Ship ship;

	float rot = 0.0f;

public:
	bool GetRunning() const { return running; }


	void Update(float dt);

	void OnInput(SDL_Event &event);


	void Render();

	void ResizeWindow(int w, int h);

};


