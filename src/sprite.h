//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include "surface.h"
#include "camera.h"
#include "transform.h"

#include <SDL_rect.h>

class Sprite
{
public:
	Sprite(Surface &surface, int x, int y, int w, int h, float zoom);

private:
	Surface * surface_ref = nullptr;
	float zoom;
	SDL_Rect src_rect;
	SDL_Point center;
	SDL_Rect dest_rect;

public:

	void Render(Camera &cam, float x, float y, float rot);
	void Render(Camera &cam, float x, float y, float rot, const Transform & parent_transform);

	void Render_Simple(int x, int y);

};


