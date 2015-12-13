
#pragma once


#include <SDL_events.h>

#include "camera.h"
#include "ship.h"

#include <vector>
#include <memory>

class Game
{
public:
	Game();

private:
	bool running = true;

	Camera world_cam;
	Camera screen_cam;

	Ship player_ship;

	std::vector<std::unique_ptr<Ship>> ship_list;

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


	void SetupLevel();


	void AttachPartToShip(const std::string &part_def);
	void DeleteShipPart();

	void InvalidateShip(Ship *about_to_delete);

	void SpawnRandomShip();

	void SpawnShip(const std::string &name, float x, float y, float rot);

	void CheckForDeadShips();
};


