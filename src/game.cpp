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
#include <glm/trigonometric.hpp>
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

	//if (key == SDLK_r) rotating = not rotating;
	//if (key == SDLK_t) translating = not translating;
	if (key == SDLK_n) SpawnRandomShip();
	if (key == SDLK_j) SpawnRandomJunk();

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

	CheckAndPopulateRandomShips();

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


	//Update camera
	glm::vec2 track = player_ship.GetWorldPosition() + player_ship.GetWorldPosition() + mouse_world_cursor;
	track /= 3.0f;
	//track = player_ship.GetWorldPosition();
	world_cam.SetTracking(-track);
	world_cam.UpdateTracking(dt);


	hud.Update(dt);

}



void Game::Render()
{
	RenderColour("background");
	SDL_RenderClear(RENDERER);

	//Other ships

	for(auto &ship : ship_list)
	{
		if (ship.get() == ship_cursor)
		{
			//ship->RenderShipSelected(world_cam);
		}

		ship->Render(world_cam, false, (mode == Mode::Scavenge) );
	}


	//Player ship

	if (player_ship.part_cursor and (mode == Mode::Scavenge))
	{
		player_ship.RenderShipSelected(world_cam);

		player_ship.Render(world_cam, (bool)locked_on_part_cursor, false);

		if (showing_scavenge_hints > 0)
		{
			auto worldpos = player_ship.GetTransform().GetWorldPosition({0, 0});
			worldpos.y += player_ship.GetBoundingCircle();
			auto screenpos = world_cam.WorldToScreen(worldpos.x, worldpos.y);

			if (not locked_on_part_cursor)
			{
				hud.RenderWarning_SelectPartFirst(screenpos);
			}
			else
			{
				hud.RenderWarning_SelectPartJoin(screenpos);
			}
		}
	}
	else
	{
		player_ship.Render(world_cam, false, false);
	}


	//Tractors
	RenderTractors();

	//Projectiles
	RenderProjectiles();


	//HUD

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

		RenderColour("blue");
		RenderCircle(world_cam, world_pos.x, world_pos.y, sinf(wallclock*4) * 48.0f);
		RenderCircle(world_cam, world_pos.x, world_pos.y, sinf(wallclock*4.1) * 32.0f);

		RenderColour("white");
		RenderCircle(world_cam, world_pos.x, world_pos.y, sinf(wallclock*4) * 28.0f);

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
		FireWeapons(1);
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
		FireWeapons(2);
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
		AttachShipHere(locked_on_ship_cursor, locked_on_part_cursor);
	}

}


void Game::DeleteShipPart()
{
	player_ship.DeletePartAtCursor();
	if (ship_cursor) ship_cursor->DeletePartAtCursor();
}


void Game::AttachShipHere(Ship *other_ship, Part *other_part)
{
	if (not other_ship) return;
	if (not other_part) return;
	if (not player_ship.connector_cursor) return;
	if (player_ship.connector_cursor->is_connected) return;

	//Coudln't get multiple part ship connections working .. running out of time, so just attach single parts

	//Destroy surrounding parts
	other_ship->DeletePartsAroundPart(other_part);

	player_ship.AttachPartAtCursor(other_part->GetName(), true);

	//glm::vec2 attach_pos { connector_cursor->x, connector_cursor->y };
	//float attach_rot = connector_cursor->rot;

	//AddPart(part->GetName(), attach_pos.x, attach_pos.y, attach_rot);
	//part_list.back()->SetIsland(1);

	//other_ship->enable_clipping = false;

	glm::vec2 start_pos = other_ship->GetWorldPositionPart(other_part);
	glm::vec2 end_pos = player_ship.GetWorldPositionConnection(player_ship.connector_cursor);

	tractor_list.emplace_back(new TractorBeam{other_part, start_pos, end_pos});

	//other_ship->part_list.clear();
	other_ship->DeletePart(other_part);

	//RecalcConnections();
	//RecalcCenterOfGravity();

	other_ship->RecalcConnections();
	other_ship->RecalcCenterOfGravity();

	locked_on_ship_cursor = nullptr;
	locked_on_part_cursor = nullptr;

	showing_scavenge_hints--;

}



void Game::SetupLevel()
{
	const int num_ships = 1;
	for (int i = 0; i < num_ships; i++)
	{
		SpawnRandomShip();
	}

	const int num_junk = 1;
	for (int i = 0; i < num_junk; i++)
	{
		SpawnRandomJunk();
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



void Game::SpawnRandomJunk()
{
	std::vector<std::string> part_names { "pointy", "armour", "scaffold", "laser", "launcher" };
	int n = rand() % part_names.size();

	float x = rand() % 2000 - 1000;
	float y = rand() % 2000 - 1000;
	float rot = rand() % 360;

	std::unique_ptr<Ship> ship { new Ship() };
	ship->GetTransform().SetPosition(x, y);
	ship->GetTransform().SetRotation(rot);
	ship->SetHeading(rot);

	ship->AddPart(part_names.at(n), 0.0f, 0.0f, 0.0f, false);
	ship->RecalcConnections();
	ship->RecalcCenterOfGravity();


	ship_list.push_back(std::move(ship));
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


void Game::UpdateTractors(float dt)
{
	for (auto & tractor : tractor_list)
	{
		tractor->Update(dt);
	}

	for(auto it = tractor_list.begin(); it != tractor_list.end(); )
	{
		if ((*it)->finished)
		{
			it = tractor_list.erase(it);
		}
		else
		{
			it++;
		}
	}
}


void Game::RenderTractors()
{
	for (auto & tractor : tractor_list)
	{
		tractor->Render(world_cam);
	}
}


void Game::UpdateProjectiles(float dt)
{
	for (auto & projectile : projectile_list)
	{
		projectile->Update(dt);
	}

	//TODO collisions

	for(auto it = projectile_list.begin(); it != projectile_list.end(); )
	{
		if ((*it)->ShouldRemove())
		{
			it = projectile_list.erase(it);
		}
		else
		{
			it++;
		}
	}
}


void Game::RenderProjectiles()
{
	for (auto & proj : projectile_list)
	{
		proj->Render(world_cam);
	}
}


void Game::SpawnProjectile(const std::string &name, glm::vec2 pos, glm::vec2 vel)
{


	std::unique_ptr<Projectile> proj { new Projectile(name, pos, vel, 32.0f, 2.0f)};

	projectile_list.push_back(std::move(proj));

}


void Game::InvalidateShipCursor()
{
	if (ship_cursor)
	{
		ship_cursor->InvalidateCursor();
	}

	ship_cursor = nullptr;

	//locked_on_ship_cursor = nullptr;
	//locked_on_part_cursor = nullptr;
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

	UpdateTractors(dt);

	UpdateProjectiles(dt);
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


	for (auto &proj : projectile_list)
	{
		for (auto &ship : ship_list)
		{
			ship->CheckCollision(proj.get());
		}
	}
}


void Game::SetMode(Mode new_mode)
{
	mode = new_mode;

	InvalidateShipCursor();
	locked_on_ship_cursor = nullptr;
	locked_on_part_cursor = nullptr;

	if (mode == Mode::Scavenge)
	{
		rotating = false;
	}
	else if (mode == Mode::Combat)
	{
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


void Game::FireWeapons(int weapgroup)
{
	std::string proj_name = weapgroup == 1 ? "laser" : "missile";

	std::string part_name = weapgroup == 1 ? "laser" : "launcher";

	for (const auto &part : player_ship.GetParts())
	{
		if (part->GetName() != part_name) continue;

		glm::vec2 start = player_ship.GetWorldPositionPart(part.get());

		glm::vec2 vel;

		if (weapgroup == 1)
		{
			float angle = part->GetRot() + player_ship.GetTransform().GetRotation() - 90.0f;
			vel.x = cosf(glm::radians(angle));
			vel.y = sinf(glm::radians(angle));
		}
		else
		{
			glm::vec2 target = mouse_world_cursor;
			vel = target - start;
		}

		SpawnProjectile(proj_name, start, vel);

	}

}



void Game::CheckAndPopulateRandomShips()
{
	int num_ships = 0;
	int num_junk = 0;

	for (auto &ship : ship_list)
	{
		if (ship->IsShip()) num_ships++;
		if (ship->IsJunk()) num_junk++;
	}

	hud.UpdateShipCount(num_ships, num_junk);


}

