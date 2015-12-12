//
// Created by Tristan on 12/12/2015.
//

#include "game.h"

#include "globals.h"

#include "assert.h"

#include "assets.h"
#include "ship.h"


Game::Game()
{
	assert(RENDERER);

	assert(GAME == nullptr);
	GAME = this;


}

void Game::Update(float dt)
{
	rot += dt * 36.0f;

	ship.GetTransform().SetRotation(rot);

}

void Game::OnInput(SDL_Event &event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_w) world_cam.SetOffsetRelative(0, 10.0f);
		if (event.key.keysym.sym == SDLK_s) world_cam.SetOffsetRelative(0, -10.0f);
		if (event.key.keysym.sym == SDLK_a) world_cam.SetOffsetRelative(10.0f, 0);
		if (event.key.keysym.sym == SDLK_d) world_cam.SetOffsetRelative(-10.0f, 0);

		if (event.key.keysym.sym == SDLK_KP_PLUS) world_cam.SetZoomRelative(0.25f);
		if (event.key.keysym.sym == SDLK_KP_MINUS) world_cam.SetZoomRelative(-0.25f);
	}
}


void Game::Render()
{
	SDL_RenderClear(RENDERER);


	Sprite &s = ASSETS->GetSprite("scaffold");

	s.Render_Simple(100, 0);


	ship.Render(world_cam);

	//rot = 0;
	//s.Render(cam, 0.0f, 0.0f, rot);
	//s.Render(cam, 64.0f, 0.0f, rot);
	//s.Render(cam, -64.0f, 0.0f, rot);
	//s.Render(cam, 0.0f, 64.0f, rot);
	//s.Render(cam, 0.0f, -64.0f, rot);



	SDL_RenderPresent(RENDERER);
}


void Game::ResizeWindow(int w, int h)
{
	world_cam.ViewPort(0, 0, w, h);
}

