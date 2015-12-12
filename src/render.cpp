//
// Created by Tristan on 12/12/2015.
//

#include "render.h"

#include "globals.h"

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


void RenderColour(int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(RENDERER, r, g, b, a);
}


void RenderColour(const std::string &col_name)
{
	if (col_name == "background") RenderColour(10, 20, 30, 255);
	else if (col_name == "hud_selected_part" or col_name == "hud_blue") RenderColour(128, 128, 255, 255);
	else if (col_name == "hud_collision" or col_name == "connector_red") RenderColour(255, 32, 32, 255);
	else if (col_name == "connector_grey") RenderColour(192, 192, 192, 255);
	else if (col_name == "connector_white") RenderColour(255, 255, 255, 255);
	else if (col_name == "connector_green") RenderColour(32, 255, 32, 255);

	else
	{
		std::cout << "Unknown colour name: " <<  col_name;
		RenderColour(255, 0, 255, 255);
	}
}
