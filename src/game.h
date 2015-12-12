
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
	Camera screen_cam;

	Ship ship;

	float rot = 0.0f;

	bool rotating = false;
	bool translating = false;

	glm::ivec2 mouse_cursor;
	glm::vec2 mouse_world_cursor;

public:
	bool GetRunning() const { return running; }


	void Update(float dt);

	void OnInput(SDL_Event &event);

	void OnKeyDown(SDL_Keycode key);
	void OnKeyUp(SDL_Keycode key);


	void OnMouseMove(int x, int y);
	void OnMouseDown(int x, int y, int button);
	void OnMouseUp(int x, int y, int button);

	void OnMouseWheel(int y);

	void SetMouseCursor(int x, int y);

	void Render();

	void ResizeWindow(int w, int h);



	void AttachPartToShip(const std::string &part_def);

};


