
#pragma once


#include <SDL_events.h>

#include "camera.h"
#include "ship.h"
#include "hud.h"
#include "globals.h"
#include "tractorbeam.h"
#include "projectile.h"

#include <vector>
#include <memory>

class WASD
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	glm::vec2 GetVec() const;
};


class Star
{
public:
	glm::vec2 pos;
	SDL_Color colour;
};



class Game
{
public:
	Game();

private:
	bool running = true;

	Camera world_cam;
	Camera screen_cam;

	Mode mode = Mode::Combat;
	HUD hud;

	Ship player_ship;

	std::vector<std::unique_ptr<Ship>> ship_list;

	std::vector<std::unique_ptr<TractorBeam>> tractor_list;

	std::vector<std::unique_ptr<Projectile>> projectile_list;

	std::vector<std::unique_ptr<Star>> star_list;


	float rot = 0.0f;
	float wallclock = 0.0f;

	bool rotating = true;
	bool translating = true;
	WASD arrow_controls;

	glm::ivec2 mouse_cursor;
	glm::vec2 mouse_world_cursor;

	Ship * ship_cursor = nullptr;

	Ship * locked_on_ship_cursor = nullptr;
	Part * locked_on_part_cursor = nullptr;

	int showing_scavenge_hints = 5;

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

	void DoCommand1();
	void DoCommand2();

	void AttachPartToShip();
	void DeleteShipPart();

	void AttachShipHere(Ship *ship_cursor, Part *conn);

	void InvalidateShip(Ship *about_to_delete);

	glm::vec2 GetSpawnLocation(glm::vec2 offset, float radius);

	void SpawnRandomShip(float radius = 1000.0f);
	void SpawnRandomJunk(float radius = 1000.0f);

	void SpawnShip(const std::string &name, float x, float y, float rot);

	void CheckForSplitShips();

	void CheckForDeadShips();

	void UpdateTractors(float dt);
	void RenderTractors();

	void SpawnProjectile(const std::string &name, glm::vec2 pos, glm::vec2 vel, Ship *ignore_ship);
	void UpdateProjectiles(float dt);
	void RenderProjectiles();


	void SetShipCursor();

	void InvalidateShipCursor();

	void UpdateMoveables(float dt);

	void CheckForCollisions(float dt);

	void SwitchInputMode();

	void SetMode(Mode new_mode);

	void FireWeapons(Ship &ship, int weapgroup, glm::vec2 target);


	float timer_ship_spawn = 1.0f;
	float timer_junk_spawn = 0.5f;
	bool running_ai = true;

	void CheckAndPopulateRandomShips(float dt);

	void UpdateStars(float dt);

	void RenderStars();
};


