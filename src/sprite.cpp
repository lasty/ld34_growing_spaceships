//
// Created by Tristan on 12/12/2015.
//

#include "sprite.h"
#include "globals.h"


Sprite::Sprite(Surface &surface, int x, int y, int w, int h, float zoom)
{
	surface_ref = &surface;
	src_rect.x = x;
	src_rect.y = y;
	src_rect.w = w;
	src_rect.h = h;

	this->zoom = zoom;

	dest_rect = src_rect;

	center.x = int(src_rect.w * zoom / 2.0f);
	center.y = int(src_rect.w * zoom / 2.0f);

	dest_rect.w = int(zoom * src_rect.w);
	dest_rect.h = int(zoom * src_rect.h);
}


void Sprite::Render(Camera &cam, float x, float y, float rot)
{
	SDL_Rect dest = cam.WorldToScreen(x - center.x, y - center.y, dest_rect.w, dest_rect.h);

	SDL_RenderCopyEx(RENDERER, surface_ref->GetTexture(), &src_rect, &dest, rot, nullptr, SDL_FLIP_NONE);
}


void Sprite::Render_Simple(int x, int y)
{
	//Renders from top/left at no rotation (for screen aligned text, etc)

	SDL_Rect dest = dest_rect;

	dest.x = x;
	dest.y = y;

	SDL_RenderCopy(RENDERER, surface_ref->GetTexture(), &src_rect, &dest);
}

