//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include "camera.h"

#include <string>

void RenderCircle(Camera &cam, float x, float y, float radius);
void RenderCircleRotated(Camera &cam, float x, float y, float radius, float rot);
void RenderLine(Camera &cam, float x1, float y1, float x2, float y2);

void RenderColour(const SDL_Colour &c);
void RenderColour(const std::string &col_name);

