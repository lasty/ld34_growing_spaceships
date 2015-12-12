//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include "camera.h"

#include <string>

void RenderCircle(Camera &cam, float x, float y, float radius);
void RenderLine(Camera &cam, float x1, float y1, float x2, float y2);

void RenderColour(int r, int g, int b, int a);
void RenderColour(const std::string &col_name);

