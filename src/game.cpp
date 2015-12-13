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
#include <glm/geometric.hpp>
#include <iostream>


glm::vec2 WASD::GetVec() const
{
	glm::vec2 v;
	if (up) v.y -= 1;
	if (down) v.y += 1;
	if (left) v.x -= 1;
	if (right) v.x += 1;

	return v;
}


Game::Game()
: player_ship("custom")
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
	if (key == SDLK_w or key == SDLK_UP) arrow_controls.up = true;
	if (key == SDLK_s or key == SDLK_DOWN) arrow_controls.down = true;
	if (key == SDLK_a or key == SDLK_LEFT) arrow_controls.left = true;
	if (key == SDLK_d or key == SDLK_RIGHT) arrow_controls.right = true;

	if (key == SDLK_KP_PLUS) world_cam.SetZoomRelative(0.25f);
	if (key == SDLK_KP_MINUS) world_cam.SetZoomRelative(-0.25f);

	if (key == SDLK_r) rotating = not rotating;
	if (key == SDLK_t) translating = not translating;
	if (key == SDLK_n) SpawnRandomShip();

	if (key == SDLK_TAB or key == SDLK_SPACE or key == SDLK_KP_0 or key == SDLK_RETURN or key == SDLK_KP_ENTER)
	{
		SwitchInputMode();
	}


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
	if (key == SDLK_w or key == SDLK_UP) arrow_controls.up = false;
	if (key == SDLK_s or key == SDLK_DOWN) arrow_controls.down = false;
	if (key == SDLK_a or key == SDLK_LEFT) arrow_controls.left = false;
	if (key == SDLK_d or key == SDLK_RIGHT) arrow_controls.right = false;

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

	if (button == 1) DoCommand1();
	else if (button == 3) DoCommand2();

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
	wallclock += dt;

	UpdateMoveables(dt);


	CheckForCollisions(dt);


	CheckForSplitShips();

	CheckForDeadShips();


	if (rotating)
	{
		player_ship.SetHeading(mouse_world_cursor);
	}

	if (translating)
	{
		player_ship.SetThrust(arrow_controls.GetVec());
	}


	SetShipCursor();

	if (ship_cursor)
	{
		ship_cursor->SetPartCursor(mouse_world_cursor);
		ship_cursor->SetConnectorCursor(mouse_world_cursor);
	}


	player_ship.SetPartCursor(mouse_world_cursor);
	player_ship.SetConnectorCursor(mouse_world_cursor);


	hud.Update(dt);

}



void Game::Render()
{
	RenderColour("background");
	SDL_RenderClear(RENDERER);


	//Sprite &s = ASSETS->GetSprite("scaffold");
	//s.Render_Simple(100, 0);


	for(auto &ship : ship_list)
	{
		if (ship.get() == ship_cursor)
		{
			ship->RenderShipSelected(world_cam);
		}

		ship->Render(world_cam);
	}

	if (player_ship.part_cursor)
		player_ship.RenderShipSelected(world_cam);

	player_ship.Render(world_cam);


	RenderColour("blue");
	//s.Render_Simple(mouse_cursor.x, mouse_cursor.y);

	RenderLine(screen_cam, mouse_cursor.x - 10, mouse_cursor.y, mouse_cursor.x + 10, mouse_cursor.y);
	RenderLine(screen_cam, mouse_cursor.x, mouse_cursor.y - 10, mouse_cursor.x, mouse_cursor.y + 10);


	//RenderCircle(world_cam, mouse_world_cursor.x, mouse_world_cursor.y, 64.0f);
	//s.Render(world_cam, mouse_world_cursor.x, mouse_world_cursor.y, 0.0f);


	if (locked_on_ship_cursor and locked_on_part_cursor)
	{
		const glm::vec2 connector_pos = locked_on_part_cursor->GetOffset();

		const glm::vec2 world_pos = locked_on_ship_cursor->GetTransform().GetWorldPosition(connector_pos);

		RenderCircle(world_cam, world_pos.x, world_pos.y, sinf(wallclock*4) * 32.0f);
	}


	hud.Render();


	SDL_RenderPresent(RENDERER);
}


void Game::ResizeWindow(int w, int h)
{
	world_cam.ViewPort(0, 0, w, h);

	screen_cam.SetToScreen(0, 0, w, h);

	hud.Resize(w, h);
}


void Game::DoCommand1()
{
	if (mode == Mode::Combat)
	{
		DeleteShipPart();  //TODO
	}
	else if (mode == Mode::Scavenge)
	{
		AttachPartToShip();
	}
}


void Game::DoCommand2()
{
	if (mode == Mode::Combat)
	{
		DeleteShipPart();  //TODO
	}
	else if (mode == Mode::Scavenge)
	{
		DeleteShipPart();
	}
}


void Game::AttachPartToShip()
{
	//player_ship.AttachPartAtCursor(part_def);

	if (ship_cursor)  //if targeting other ship
	{
		locked_on_ship_cursor = ship_cursor;
		locked_on_part_cursor = ship_cursor->part_cursor;
	}
	else  //else attaching to player ship
	{
		player_ship.AttachShipHere(locked_on_ship_cursor, locked_on_part_cursor);
	}

}


void Game::DeleteShipPart()
{
	player_ship.DeletePartAtCursor();
	if (ship_cursor) ship_cursor->DeletePartAtCursor();
}


void Game::SetupLevel()
{
	const int num_ships = 10;
	for (int i = 0; i < num_ships; i++)
	{
		SpawnRandomShip();
	}

	world_cam.SetZoom(2.0f);

	SetMode(Mode::Scavenge);

}


void Game::SpawnRandomShip()
{
	std::vector<std::string> ship_names { "pointy", "ship_one", "custom"};
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
	ship->SetHeading(rot);

	ship_list.push_back(std::move(ship));

}


void Game::InvalidateShip(Ship *about_to_delete)
{
	//Don't call invalidate cursor here, in case data is already deleted
	if (ship_cursor == about_to_delete)
	{
		ship_cursor = nullptr;
	}

	if (locked_on_ship_cursor == about_to_delete)
	{
		locked_on_ship_cursor = nullptr;
		locked_on_part_cursor = nullptr;
	}
}


void Game::CheckForSplitShips()
{
	std::vector<std::unique_ptr<Ship>> new_ship_list;

	for(auto & ship : ship_list)
	{
		while (ship->ShouldSplit())
		{
			new_ship_list.push_back( ship->SplitShip() );
		}
	}


	if (player_ship.ShouldSplit())
	{
		new_ship_list.push_back( player_ship.SplitShip() );
	}


	for(auto &ship : new_ship_list)
	{
		ship_list.push_back(std::move(ship));
	}

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


void Game::InvalidateShipCursor()
{
	if (ship_cursor)
	{
		ship_cursor->InvalidateCursor();
	}

	ship_cursor = nullptr;
}


void Game::SetShipCursor()
{
	InvalidateShipCursor();

	if (ship_list.size() == 0)
	{
		return;
	}

	float dist_min = 10000.0f;

	for (auto & ship : ship_list)
	{
		const glm::vec2 ship_pos { ship->GetTransform().GetPosition().x, ship->GetTransform().GetPosition().y };
		float dist = glm::distance(ship_pos, mouse_world_cursor);

		if (not ship_cursor or (dist < dist_min))
		{
			if (dist < (ship->GetBoundingCircle() + 0))
			{
				dist_min = dist;
				ship_cursor = ship.get();
			}
		}
	}
}


void Game::UpdateMoveables(float dt)
{
	player_ship.Update(dt);

	for (auto & ship : ship_list)
	{
		ship->Update(dt);
	}
}


void Game::CheckForCollisions(float dt)
{
	for (auto &ship1 : ship_list)
	{
		player_ship.CheckCollision(ship1.get(), dt);
		ship1->CheckCollision(&player_ship, dt);

		for (auto &ship2 : ship_list)
		{
			if (ship1 == ship2) continue;

			ship1->CheckCollision(ship2.get(), dt);
		}

	}
}


void Game::SetMode(Mode new_mode)
{
	mode = new_mode;

	if (mode == Mode::Scavenge)
	{
		InvalidateShipCursor();
		rotating = false;
	}
	else if (mode == Mode::Combat)
	{
		InvalidateShipCursor();
		rotating = true;
	}

	hud.SetMode(mode);
}


void Game::SwitchInputMode()
{
	if (mode == Mode::Scavenge)
	{
		SetMode(Mode::Combat);
	}
	else
	{
		SetMode(Mode::Scavenge);
	}
}
