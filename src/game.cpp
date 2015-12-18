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
#include <glm/gtc/random.hpp>
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
: player_ship("core")
//: player_ship("pointy")
//: player_ship("core")
{
	assert(RENDERER);

	assert(GAME == nullptr);
	GAME = this;

	world_cam.SetZoom(3.0f);

	NewGame(true);
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

	if (key == SDLK_F1)
	{
		running_ai = not running_ai;
	}


	if (key == SDLK_F2)
	{
		tutorial_enabled = false;
		NewGame(false);
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

	CheckAndPopulateRandomShips(dt);
	UpdateStars(dt);

	CheckForCollisions(dt);

	CheckForSplitShips();

	CheckForDeadShips();

	//Update screen position, camera might have moved
	SetMouseCursor(mouse_cursor.x, mouse_cursor.y);

	if (rotating)
	{
		player_ship.SetHeading(mouse_world_cursor);
	}

	if (translating)
	{
		player_ship.SetThrust(arrow_controls.GetVec());
	}
	else
	{
		//set thrust to 0, or ships will keep drifting in last set velocity
		player_ship.SetThrust({0.0, 0.0});
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

	//Adjust for GUI at the bottom of the screen
	track -= world_cam.ScreenToWorld(0, 0) - world_cam.ScreenToWorld(0, hud.GetHudSize() / 2);

	//track = player_ship.GetWorldPosition();
	//std::cout << "player ship pos = " << player_ship.GetWorldPosition().x << "  ,  " << player_ship.GetWorldPosition().y << std::endl;
	world_cam.SetTracking(-track);
	world_cam.UpdateTracking(dt);


	if (tutorial_enabled) CheckTutorialConditions();

	CheckForGameOver();

	hud.Update(dt);

}



void Game::Render()
{
	RenderColour("background");
	SDL_RenderClear(RENDERER);

	//Background starfield
	RenderStars();

	//Other ships

	for (auto &ship : ship_list)
	{
		if (ship.get() == ship_cursor)
		{
			//ship->RenderShipSelected(world_cam);
		}

		ship->Render(world_cam, false, (mode == Mode::Scavenge));
	}


	//Player ship

	if (player_ship.part_cursor and (mode == Mode::Scavenge))
	{
		player_ship.RenderShipSelected(world_cam);

		player_ship.Render(world_cam, (bool) locked_on_part_cursor, false);
	}
	else
	{
		player_ship.Render(world_cam, false, false);
	}


	//Tractors
	RenderTractors();

	//Render line from part to player ship connector
	if (locked_on_part_cursor and locked_on_ship_cursor)
	{
		glm::vec2 start_pos = locked_on_ship_cursor->GetWorldPositionPart(locked_on_part_cursor);

		if (player_ship.connector_cursor)
		{
			glm::vec2 end_pos = player_ship.GetWorldPositionConnection(player_ship.connector_cursor);

			if (player_ship.connector_cursor->is_connected)
				RenderColour("red");
			else
				RenderColour("tractorbeam");

			RenderLine(world_cam, end_pos.x, end_pos.y, start_pos.x, start_pos.y);
		}
		else
		{
			RenderColour("blue");

			RenderLine(world_cam, mouse_world_cursor.x, mouse_world_cursor.y, start_pos.x, start_pos.y);
		}


	}

	//Projectiles
	RenderProjectiles();


	//cursors


	if (mode == Mode::Combat)
	{
		RenderColour("red");
		//RenderLine(screen_cam, mouse_cursor.x - 10, mouse_cursor.y, mouse_cursor.x + 10, mouse_cursor.y);
		//RenderLine(screen_cam, mouse_cursor.x, mouse_cursor.y - 10, mouse_cursor.x, mouse_cursor.y + 10);

		//render animated circle crosshair
		RenderCircle(screen_cam, mouse_cursor.x, mouse_cursor.y, 16);
		RenderCircle(screen_cam, mouse_cursor.x, mouse_cursor.y, cosf( fmodf(wallclock*3.0f, glm::radians(90.0f)) ) * 15);
	}


	if (mode == Mode::Scavenge)
	{
		//Render salvage cursor
		glm::vec2 salvage_cursor_world_pos;

		if (locked_on_ship_cursor and locked_on_part_cursor)
		{
			const glm::vec2 connector_pos = locked_on_part_cursor->GetOffset();
			salvage_cursor_world_pos = locked_on_ship_cursor->GetTransform().GetWorldPosition(connector_pos);
		}
		else
		{
			salvage_cursor_world_pos = mouse_world_cursor;
		}

		RenderColour("blue");
		RenderCircle(world_cam, salvage_cursor_world_pos.x, salvage_cursor_world_pos.y, sinf(wallclock * 4) * 48.0f);
		RenderCircle(world_cam, salvage_cursor_world_pos.x, salvage_cursor_world_pos.y, sinf(wallclock * 4.1) * 32.0f);

		RenderColour("white");
		RenderCircle(world_cam, salvage_cursor_world_pos.x, salvage_cursor_world_pos.y, sinf(wallclock * 4) * 28.0f);

	}


	//HUD
	hud.Render();

	if (tutorial_enabled)
	{
		hud.RenderTutorial();
	}


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
		FireWeapons(player_ship, 1, mouse_world_cursor);
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
		FireWeapons(player_ship, 2, mouse_world_cursor);
	}
	else if (mode == Mode::Scavenge)
	{
		DeleteShipPart();
	}
}


void Game::AttachPartToShip()
{
	//player_ship.AttachPartAtCursor(part_def);

	if (player_ship.connector_cursor)
	{
		AttachShipHere(locked_on_ship_cursor, locked_on_part_cursor);

	}
	else
	if (ship_cursor)  //if targeting other ship
	{
		if (ship_cursor->IsJunk())  //only strip parts of destroyed ships
		{
			locked_on_ship_cursor = ship_cursor;
			locked_on_part_cursor = ship_cursor->part_cursor;

			PlayWorldSound("select", ship_cursor->GetWorldPosition());
		}
		else
		{
			PlayWorldSound("error", ship_cursor->GetWorldPosition());
		}
	}
	//else  //else attaching to player ship
	//{
	//}

}


void Game::DeleteShipPart()
{
	//Reworking this to just detach player ship parts without destroying them
	player_ship.DetachPartAtCursor();

	//if (ship_cursor) ship_cursor->DeletePartAtCursor();
}


void Game::AttachShipHere(Ship *other_ship, Part *other_part)
{
	if (not other_part or not other_ship)
	{
		PlayWorldSound("error", player_ship.GetWorldPosition());
	}

	if (not other_ship) return;
	if (not other_part) return;

	if (not player_ship.connector_cursor) { PlayWorldSound("error", player_ship.GetWorldPosition()); return; }
	if (player_ship.connector_cursor->is_connected) { PlayWorldSound("error", player_ship.GetWorldPosition()); return; }


	//Couldn't get multiple part ship connections working .. running out of time, so just attach single parts


	//Detach ship part
	other_part->DisconnectAll();

	player_ship.AttachPartAtCursor(other_part->GetName(), true);
	PlayWorldSound("select", player_ship.GetWorldPosition());

	//glm::vec2 attach_pos { connector_cursor->x, connector_cursor->y };
	//float attach_rot = connector_cursor->rot;

	//AddPart(part->GetName(), attach_pos.x, attach_pos.y, attach_rot);
	//part_list.back()->SetIsland(1);

	//other_ship->enable_clipping = false;

	glm::vec2 start_pos = other_ship->GetWorldPositionPart(other_part);
	glm::vec2 end_pos = player_ship.GetWorldPositionConnection(player_ship.connector_cursor);

	tractor_list.emplace_back(new TractorBeam{other_part, start_pos, end_pos});
	PlayWorldSound("tractorbeam", start_pos);

	//other_ship->part_list.clear();
	other_ship->DeletePart(other_part);

	//RecalcConnections();
	//RecalcCenterOfGravity();

	other_ship->RecalcConnections();
	other_ship->RecalcCenterOfGravity();

	locked_on_ship_cursor = nullptr;
	locked_on_part_cursor = nullptr;

}


void Game::NewGame(bool run_tutorial)
{
	//invalidate cursors
	InvalidateShipCursor();
	locked_on_ship_cursor = nullptr;
	locked_on_part_cursor = nullptr;

	//Get ship name, and setup tutorial
	std::string whatship;

	if (run_tutorial)
	{
		whatship = "pointy";
		tutorial_enabled = true;
		timer_ship_spawn = 2.0f;
	}
	else
	{
		timer_ship_spawn = 8.0f;
		whatship = ASSETS->GetRandomShipName();
	}

	//Reset Player Ship
	player_ship.Load(whatship);

	player_ship.GetTransform().SetPosition(0.0f, 0.0f);
	player_ship.GetTransform().SetRotation(0.0f);

	//Reset Camera
	world_cam.SetOffset(0.0f, 0.0f);

	//Reset world
	ship_list.clear();
	star_list.clear();
	tractor_list.clear();
	projectile_list.clear();

	//call start up for level
	SetupLevel();
}


void Game::CheckForGameOver()
{
	if (player_ship.IsJunk() and mode != Mode::GameOver)
	{
		SetMode(Mode::GameOver);
	}
}

void Game::SetupLevel()
{
	//const int num_ships = 1;
	//for (int i = 0; i < num_ships; i++)
	//{
	//	SpawnRandomShip();
	//}

	const int num_junk = 15;
	for (int i = 0; i < num_junk; i++)
	{
		SpawnRandomJunk();
	}


	if (tutorial_enabled)
	{
		SetTutorial(1);
	}


	SetMode(Mode::Combat);

}

glm::vec2 Game::GetSpawnLocation(glm::vec2 offset, float radius)
{
	glm::vec2 circ = glm::circularRand(radius);
	circ += offset;

	circ += glm::circularRand(radius / 10.0f);

	return circ;
}

void Game::SpawnRandomShip(float radius)
{
	std::string ship_name = ASSETS->GetRandomShipName();

	glm::vec2 pos = GetSpawnLocation(player_ship.GetWorldPosition(), radius);

	float rot = rand() % 360;

	SpawnShip(ship_name, pos.x, pos.y, rot);
}



void Game::SpawnRandomJunk(float radius)
{
	std::vector<std::string> part_names { "pointy", "armour", "scaffold", "laser", "launcher" };
	int n = rand() % part_names.size();

	glm::vec2 pos = GetSpawnLocation(player_ship.GetWorldPosition(), radius);
	float rot = rand() % 360;

	std::unique_ptr<Ship> ship { new Ship() };
	ship->GetTransform().SetPosition(pos.x, pos.y);
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


void Game::SpawnProjectile(const std::string &name, glm::vec2 pos, glm::vec2 vel, Ship *ignore_ship)
{
	std::unique_ptr<Projectile> proj { new Projectile(name, pos, vel, 32.0f, 2.0f, ignore_ship)};

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
			if (dist < (ship->GetBoundingCircle() + 400))
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

		if (ship->IsShip())  //dont run AI on junk
		{
			if (running_ai)
			{
				ship->EnemyShipAI(dt, player_ship.GetWorldPosition());
			}
		}
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

		player_ship.CheckCollision(proj.get());
	}
}


void Game::SetMode(Mode new_mode)
{
	mode = new_mode;

	ASSETS->GetSound("menubeep").PlayWithVolume(MASTER_VOLUME);


	InvalidateShipCursor();
	locked_on_ship_cursor = nullptr;
	locked_on_part_cursor = nullptr;

	if (mode == Mode::Scavenge)
	{
		translating = true;
		rotating = false;

		ASSETS->GetSound("scanner").PlayWithVolume(MASTER_VOLUME);

	}
	else if (mode == Mode::Combat)
	{
		translating = true;
		rotating = true;

		//ASSETS->GetSound("redalert").PlayWithVolume(MASTER_VOLUME);

	}
	else if (mode == Mode::GameOver)
	{
		rotating = false;
		translating = false;
	}

	hud.SetMode(mode);
}


void Game::SwitchInputMode()
{
	if (mode == Mode::Scavenge)
	{
		SetMode(Mode::Combat);
	}
	else if (mode == Mode::Combat)
	{
		SetMode(Mode::Scavenge);
	}
	else if (mode == Mode::GameOver)
	{
		NewGame(false);
	}
}


void Game::FireWeapons(Ship &ship, int weapgroup, glm::vec2 target)
{
	std::string proj_name;
	std::string part_name;
	std::string sound_name;

	if (weapgroup == 1)
	{
		proj_name = "laser";
		part_name = "laser";

		sound_name = ship.GetNumLasers() ? "laser" : "error";
	}
	else if (weapgroup == 2)
	{
		proj_name = "missile";
		part_name = "launcher";

		sound_name = ship.GetNumMissiles() ? "missile" : "error";
	}


	PlayWorldSound(sound_name, ship.GetWorldPosition());


	for (const auto &part : ship.GetParts())
	{
		if (part->GetName() != part_name) continue;

		glm::vec2 start = ship.GetWorldPositionPart(part.get());

		glm::vec2 vel;

		if (weapgroup == 1)
		{
			float angle = part->GetRot() + ship.GetTransform().GetRotation() - 90.0f;
			vel.x = cosf(glm::radians(angle));
			vel.y = sinf(glm::radians(angle));
		}
		else
		{
			//glm::vec2 target = mouse_world_cursor;
			vel = target - start;
		}

		SpawnProjectile(proj_name, start, vel, &ship);

	}

}



void Game::CheckAndPopulateRandomShips(float dt)
{
	if (tutorial_enabled) return;

	int num_ships = 0;
	int num_junk = 0;

	float radius_spawn = 4000.0f;
	float radius_remove = 6000.0f;
	int ship_quota = 7;
	int junk_quota = 20;


	for (auto &ship : ship_list)
	{
		if (ship->ShouldRemove()) continue;

		float distance_to_player = glm::distance(player_ship.GetWorldPosition(), ship->GetWorldPosition());
		if (distance_to_player > radius_remove)
		{
			ship->MarkForRemoval();
			continue;
		}

		if (ship->IsShip()) num_ships++;
		if (ship->IsJunk()) num_junk++;
	}

	hud.UpdateShipCount(num_ships, num_junk);

	timer_ship_spawn -= dt;

	if (num_ships < ship_quota and timer_ship_spawn <= 0.0f)
	{
		SpawnRandomShip(radius_spawn);
		timer_ship_spawn = (rand() % 100 / 100.0f) * 7.0f;
		if (num_ships <= 2) timer_ship_spawn -= 1.0f;
	}

	timer_junk_spawn -= dt;
	if (num_junk < junk_quota and timer_junk_spawn <= 0.0f)
	{
		SpawnRandomJunk(radius_spawn);
		timer_junk_spawn = (rand() % 100 / 100.0f) * 3.0f;
	}

}


Star * NewStar(glm::vec2 pos, float radius)
{
	pos += glm::diskRand(radius);

	SDL_Colour col = ASSETS->GetColour( rand() % 2 == 0 ? "white" : "grey");
	return new Star{pos, col};
}


void Game::UpdateStars(float dt)
{
	unsigned star_quota = 1000;
	float radius_remove = 5000.0f;
	float radius_add = 4000.0f;

	for(auto &star : star_list)
	{
		float distance_to_player = glm::distance(player_ship.GetWorldPosition(), star->pos);

		if (distance_to_player > radius_remove)
		{
			star.reset( NewStar(player_ship.GetWorldPosition(), radius_add) );
		}
	}

	while (star_list.size() < star_quota)
	{
		star_list.emplace_back( NewStar(player_ship.GetWorldPosition(), radius_add));
	}
}


void Game::RenderStars()
{
	for(auto &star : star_list)
	{
		RenderColour(star->colour);
		auto p = world_cam.WorldToScreen(star->pos.x, star->pos.y);
		SDL_RenderDrawPoint(RENDERER, p.x, p.y);
	}
}


void Game::PlayWorldSound(const std::string &name, const glm::vec2 &location)
{
	auto & sound =  ASSETS->GetSound(name);

	float dist = glm::distance(-world_cam.GetOffset(), location);
	if (dist == 0.0f) dist = 1.0f;

	float loudness = 2000.0f / dist;

	loudness = glm::clamp(loudness, 0.0f, 1.0f) * MASTER_VOLUME;

	sound.PlayWithVolume(loudness);
}


void Game::CheckTutorialConditions()
{
	switch(tutorial_step)
	{
		case 1:
			if (mode == Mode::Scavenge)
				SetTutorial(2);
			break;

		case 2:
			if (player_ship.CountNumParts("pointy") <= 3)
				SetTutorial(3);
			else if (mode == Mode::Combat)
				SetTutorial(1);
			break;

		case 3:
			if (locked_on_part_cursor and locked_on_part_cursor->GetName() == "laser")
				SetTutorial(4);
			break;

		case 4:

			if (player_ship.GetNumLasers())
				SetTutorial(5);
			else if (not (locked_on_part_cursor and locked_on_part_cursor->GetName() == "laser"))
				SetTutorial(3);
			break;

		case 5:
			if (player_ship.GetNumMissiles())
				SetTutorial(6);
			break;

		case 6:
			if (mode == Mode::Combat)
			{
				SetTutorial(7);

				glm::vec2 pos = GetSpawnLocation(player_ship.GetWorldPosition(), 1000.0f);
				float rot = rand() % 360;
				SpawnShip("ship_one", pos.x, pos.y, rot);
			}
			break;

		case 7:
			if (GetNumActiveShips() == 0)
			{
				SpawnRandomShip(2000.0f);
				SetTutorial(8);
			}
			break;

			case 8:
				if (GetNumActiveShips() == 0)
				{
					SetTutorial(0);
				}
			break;
	}
}


int Game::GetNumActiveShips()
{
	int num_ships = 0;
	for (auto &ship : ship_list)
	{
		if (ship->ShouldRemove()) continue;

		if (ship->IsShip()) num_ships++;
	}
	return num_ships;
}


void Game::SetTutorial(int n)
{
	constexpr int n_max = 8;

	if (n == 0 or n > n_max)
	{
		tutorial_enabled = false;
		return;
	}

	tutorial_enabled = true;
	tutorial_step = n;

	std::string tutorial1;
	std::string tutorial2;

	switch(n)
	{
		case 1:
			tutorial1 = "Your ship has no weapons...  Let's fix that.";
			tutorial2 = ">> Press [TAB] to change to SALVAGE mode";
			break;

		case 2:
			tutorial1 = "This ship is ugly, let's first remove some of those pointy triangle pieces.";
			tutorial2 = ">> [RIGHT CLICK]: Remove ship pieces to make room";
			break;

		case 3:
			tutorial1 = "Now, Find a LASER weapon (Red) floating in space, and select it";
			tutorial2 = ">> [LEFT CLICK] on a LASER weapon to lock-on with the Tractor Beam";
			break;

		case 4:
			tutorial1 = "Now find a suitable spot to attach it to on your ship.";
			tutorial2 = ">> [LEFT CLICK] on your ship.  (Take note of the orientation)";
			break;

		case 5:
			tutorial1 = "Now repeat the process with a MISSILE LAUNCHER (Blue)";
			tutorial2 = ">> Attach a missile launcher to your ship";
			break;

		case 6:
			tutorial1 = "Congratulations you are now armed.  You can also try out some armour if you like";
			tutorial2 = ">> Press [TAB] to return to COMBAT mode";
			break;

		case 7:
			tutorial1 = "Now for some target practice.  Ships will keep flying until their core is destroyed.";
			tutorial2 = ">> [LEFT CLICK] Fire Lasers.  [RIGHT CLICK] Fire missiles.";
			break;

		case 8:
			tutorial1 = "Here they come, Good Luck!";
			tutorial2 = ">> [Mouse Scroll Wheel] Zoom in/out.  [W][A][S][D] Move Ship.";
			break;
	}

	hud.SetTutorial(n, n_max, tutorial1, tutorial2);
}
