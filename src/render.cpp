//
// Created by Tristan on 12/12/2015.
//

#include "render.h"

#include "globals.h"
#include "assets.h"

#include <SDL_render.h>

#include <glm/gtc/constants.hpp>
#include <glm/detail/func_trigonometric.hpp>

#include <iostream>

void RenderCircle(Camera &cam, float x, float y, float radius)
{
	const int segments = 32;
	for (int i = 0; i <= segments; i++)
	{
		float r1 = (i/float(segments) * glm::radians(360.0f));
		float r2 = ((i+1) / float(segments) * glm::radians(360.0f));

		float x1 = x + glm::cos(r1) * radius;
		float y1 = y + glm::sin(r1) * radius;

		float x2 = x + glm::cos(r2) * radius;
		float y2 = y + glm::sin(r2) * radius;

		RenderLine(cam, x1, y1, x2, y2);
	}
}


void RenderCircleRotated(Camera &cam, float x, float y, float radius, float rot)
{
	RenderCircle(cam, x, y, radius - 1);
	RenderCircle(cam, x, y, radius);

	for (int i = -3; i <= 3; i++)
	{
		float x2 = x + glm::cos(glm::radians(rot - 90 + i)) * radius;
		float y2 = y + glm::sin(glm::radians(rot - 90 + i)) * radius;

		RenderLine(cam, x, y, x2, y2);
	}
}


void RenderLine(Camera &cam, float x1, float y1, float x2, float y2)
{
	SDL_Point p1 = cam.WorldToScreen(x1, y1);
	SDL_Point p2 = cam.WorldToScreen(x2, y2);

	SDL_RenderDrawLine(RENDERER, p1.x, p1.y, p2.x, p2.y);
}


void RenderColour(const SDL_Color &c)
{
	SDL_SetRenderDrawColor(RENDERER, c.r, c.g, c.b, c.a);
}


void RenderColour(const std::string &col_name)
{
	const auto & col = ASSETS->GetColour(col_name);
	RenderColour(col);
}
