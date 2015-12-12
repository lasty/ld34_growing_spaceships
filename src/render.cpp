//
// Created by Tristan on 12/12/2015.
//

#include "render.h"

#include "globals.h"

#include <SDL_render.h>

#include <glm/gtc/constants.hpp>
#include <glm/detail/func_trigonometric.hpp>


void RenderCircle(Camera &cam, float x, float y, float radius)
{
	const int segments = 16;
	for (int i = 0; i <= segments; i++)
	{
		float r1 = (i/float(segments) * 2 * 3.1415f);
		float r2 = ((i+1) / float(segments) * 2 * 3.1415f);

		float x1 = x + glm::cos(r1) * radius;
		float y1 = y + glm::sin(r1) * radius;

		float x2 = x + glm::cos(r2) * radius;
		float y2 = y + glm::sin(r2) * radius;

		RenderLine(cam, x1, y1, x2, y2);
	}
}

void RenderLine(Camera &cam, float x1, float y1, float x2, float y2)
{
	SDL_Point p1 = cam.WorldToScreen(x1, y1);
	SDL_Point p2 = cam.WorldToScreen(x2, y2);

	SDL_RenderDrawLine(RENDERER, p1.x, p1.y, p2.x, p2.y);
}


void RenderColour(int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(RENDERER, r, g, b, a);
}


void RenderColour(const std::string &col_name)
{
	if (col_name == "background") RenderColour(10, 20, 30, 255);
	if (col_name == "hud1") RenderColour(128, 128, 255, 255);
	if (col_name == "hud2") RenderColour(255, 32, 32, 255);
	if (col_name == "hud3") RenderColour(192, 192, 192, 255);

}
