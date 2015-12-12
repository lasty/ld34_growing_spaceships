//
// Created by Tristan on 12/12/2015.
//

#include "game.h"

#include "globals.h"

#include "assert.h"

#include "assets.h"
#include "ship.h"
#include "render.h"


Game::Game()
: ship("ship_one")
//: ship("pointy")
//: ship("core")
{
	assert(RENDERER);

	assert(GAME == nullptr);
	GAME = this;


}


void Game::Update(float dt)
{
	if (rotating)
	{
		rot += dt * 36.0f;

		ship.GetTransform().SetRotation(rot);
	}


	if (translating) ship.GetTransform().SetPositionRelative(dt * 100.0f, 0.0f);

	ship.SetPartCursor(mouse_world_cursor);

	ship.SetConnectorCursor(mouse_world_cursor);

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
	else if (button == 3) AttachPartToShip("pointy");
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


void Game::Render()
{
	RenderColour("background");
	SDL_RenderClear(RENDERER);


	Sprite &s = ASSETS->GetSprite("scaffold");

	s.Render_Simple(100, 0);


	ship.Render(world_cam);


	RenderColour("hud1");
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
	ship.AttachPartAtCursor(part_def);
}
