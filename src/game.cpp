//
// Created by Tristan on 12/12/2015.
//

#include "game.h"

#include "globals.h"

#include "assert.h"

#include "assets.h"

#include <SDL_render.h>

float rot = 0.0f;

Camera cam;

Game::Game()
{
	assert(RENDERER);

	assert(GAME == nullptr);
	GAME = this;


}

void Game::Update(float dt)
{
	rot += dt * 36.0f;
}

void Game::OnInput(SDL_Event &event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_w) cam.SetOffsetRelative(0, 10.0f);
		if (event.key.keysym.sym == SDLK_s) cam.SetOffsetRelative(0, -10.0f);
		if (event.key.keysym.sym == SDLK_a) cam.SetOffsetRelative(10.0f, 0);
		if (event.key.keysym.sym == SDLK_d) cam.SetOffsetRelative(-10.0f, 0);

		if (event.key.keysym.sym == SDLK_KP_PLUS) cam.SetZoomRelative(0.25f);
		if (event.key.keysym.sym == SDLK_KP_MINUS) cam.SetZoomRelative(-0.25f);
	}
}


void Game::Render()
{
	SDL_RenderClear(RENDERER);

	Sprite &s = ASSETS->GetSprite("scaffold");

	s.Render_Simple(100, 0);

	//rot = 0;
	s.Render(cam, 0.0f, 0.0f, rot);
	s.Render(cam, 64.0f, 0.0f, rot);
	s.Render(cam, -64.0f, 0.0f, rot);
	s.Render(cam, 0.0f, 64.0f, rot);
	s.Render(cam, 0.0f, -64.0f, rot);



	SDL_RenderPresent(RENDERER);
}


void Game::ResizeWindow(int w, int h)
{
	cam.ViewPort(0, 0, w, h);
}

