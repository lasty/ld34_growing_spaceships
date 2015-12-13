//
// Created by Tristan on 12/12/2015.
//

#include "game.h"

#include "globals.h"

#include "assert.h"

#include "assets.h"
#include "ship.h"
#include "render.h"

#include <algorithm>

Game::Game()
: player_ship("ship_one")
//: player_ship("pointy")
//: player_ship("core")
{
	assert(RENDERER);

	assert(GAME == nullptr);
	GAME = this;

	SetupLevel();
}


void Game::OnInput(SDL_Event &event)
{
	if (event.type == SDL_KEYDOWN) OnKeyDown(event.key.keysym.sym);
	if (event.type == SDL_KEYUP) OnKeyUp(event.key.keysym.sym);

	if (event.type == SDL_MOUSEMOTION) OnMouseMove(event.motion.x, event.motion.y);
	if (event.type == SDL_MOUSEBUTTONDOWN) OnMouseDown(event.button.x, event.button.y, event.button.button);
	if (event.type == SDL_MOUSEBUTTONUP) OnMouseUp(event.button.x, event.button.y, event.button.button);

	if (event.type == SDL_MOUSEWHEEL) OnMouseWheel(event.wheel.y);

}


void Game::OnKeyDown(SDL_Keycode key)
{
	if (key == SDLK_w) world_cam.SetOffsetRelative(0, 10.0f);
	if (key == SDLK_s) world_cam.SetOffsetRelative(0, -10.0f);
	if (key == SDLK_a) world_cam.SetOffsetRelative(10.0f, 0);
	if (key == SDLK_d) world_cam.SetOffsetRelative(-10.0f, 0);

	if (key == SDLK_KP_PLUS) world_cam.SetZoomRelative(0.25f);
	if (key == SDLK_KP_MINUS) world_cam.SetZoomRelative(-0.25f);

	if (key == SDLK_r) rotating = not rotating;
	if (key == SDLK_t) translating = not translating;
	if (key == SDLK_n) SpawnRandomShip();


	if (key == SDLK_F5)
	{
		std::ofstream out{DATA_PATH+"/ships/custom.txt", std::ios::trunc};
		player_ship.Serialize(out);
	}

	if (key == SDLK_F9)
	{
		std::ifstream in{DATA_PATH+"/ships/custom.txt"};
		player_ship.Deserialize(in);
	}

}


void Game::OnKeyUp(SDL_Keycode key)
{

}


void Game::SetMouseCursor(int x, int y)
{
	mouse_cursor = {x, y};
	mouse_world_cursor = world_cam.ScreenToWorld(x, y);
}

void Game::OnMouseMove(int x, int y)
{
	SetMouseCursor(x, y);
}


void Game::OnMouseDown(int x, int y, int button)
{
	SetMouseCursor(x, y);

	if (button == 1) AttachPartToShip("scaffold");
	else if (button == 3) DeleteShipPart();
	else if (button == 2) AttachPartToShip("core");

}


void Game::OnMouseUp(int x, int y, int button)
{
	SetMouseCursor(x, y);
}


void Game::OnMouseWheel(int y)
{
	world_cam.SetZoomRelative(0.20f * -y);
	SetMouseCursor(mouse_cursor.x, mouse_cursor.y);

}


void Game::Update(float dt)
{

	//for (auto & ship : ship_list)
	//{
		//ship->Update(dt);
	//}


	CheckForDeadShips();


	if (rotating)
	{
		rot += dt * 36.0f;

		player_ship.GetTransform().SetRotation(rot);
	}


	if (translating) player_ship.GetTransform().SetPositionRelative(dt * 100.0f, 0.0f);

	player_ship.SetPartCursor(mouse_world_cursor);

	player_ship.SetConnectorCursor(mouse_world_cursor);

}



void Game::Render()
{
	RenderColour("background");
	SDL_RenderClear(RENDERER);


	//Sprite &s = ASSETS->GetSprite("scaffold");
	//s.Render_Simple(100, 0);


	for(auto &ship : ship_list)
	{
		ship->Render(world_cam);
	}

	player_ship.Render(world_cam);


	RenderColour("hud_blue");
	//s.Render_Simple(mouse_cursor.x, mouse_cursor.y);

	RenderLine(screen_cam, mouse_cursor.x - 10, mouse_cursor.y, mouse_cursor.x + 10, mouse_cursor.y);
	RenderLine(screen_cam, mouse_cursor.x, mouse_cursor.y - 10, mouse_cursor.x, mouse_cursor.y + 10);


	//RenderCircle(world_cam, mouse_world_cursor.x, mouse_world_cursor.y, 64.0f);
	//s.Render(world_cam, mouse_world_cursor.x, mouse_world_cursor.y, 0.0f);

	SDL_RenderPresent(RENDERER);
}


void Game::ResizeWindow(int w, int h)
{
	world_cam.ViewPort(0, 0, w, h);

	screen_cam.SetToScreen(0, 0, w, h);
}



void Game::AttachPartToShip(const std::string &part_def)
{
	player_ship.AttachPartAtCursor(part_def);
}


void Game::DeleteShipPart()
{
	player_ship.DeletePartAtCursor();
}


void Game::SetupLevel()
{
	SpawnRandomShip();
	SpawnRandomShip();
	SpawnRandomShip();
	world_cam.SetZoom(2.0f);
}

void Game::SpawnRandomShip()
{
	std::vector<std::string> ship_names { "pointy", "ship_one", "custom", "core"};
	int n = rand() % ship_names.size();

	float x = rand() % 2000 - 1000;
	float y = rand() % 2000 - 1000;
	float rot = rand() % 360;

	SpawnShip(ship_names.at(n), x, y, rot);
}


void Game::SpawnShip(const std::string &name, float x, float y, float rot)
{
	std::unique_ptr<Ship> ship { new Ship(name) };
	ship->GetTransform().SetPosition(x, y);
	ship->GetTransform().SetRotation(rot);

	ship_list.push_back(std::move(ship));

}


void Game::InvalidateShip(Ship *about_to_delete)
{
	//TODO remove references, eg cursor
}


void Game::CheckForDeadShips()
{
	//auto partition = std::remove_if(ship_list.begin(), ship_list.end(), [](auto &ship){ship->ShouldRemove();});

	for(auto it = ship_list.begin(); it != ship_list.end(); )
	{
		if ((*it)->ShouldRemove())
		{
			InvalidateShip(it->get());

			it = ship_list.erase(it);
		}
		else
		{
			it++;
		}
	}

}
