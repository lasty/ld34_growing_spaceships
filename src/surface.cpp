//
// Created by Tristan on 22/08/2015.
//

#include "surface.h"

// Local includes
#include "globals.h"


// SDL Includes
#include <SDL_image.h>


Surface::Surface(int width, int height)
: Surface(SDL_CreateRGBSurface(0,width,height,32,0,0,0,0))
{

}


Surface::Surface(const std::string &filename)
: Surface(IMG_Load(filename.c_str()))
{

}


Surface::Surface(SDL_Surface *ptr)
: surface(ptr, SDL_FreeSurface)
, texture(nullptr, SDL_DestroyTexture)
{
	assert(RENDERER);

	if(ptr)
	{
		UpdateSurface();
		UpdateTexture();
	}
}


void Surface::Lock()
{

}


void Surface::Unlock()
{

}


void Surface::UpdateSurface()
{
	surface_rect = SDL_Rect {0, 0, surface->w, surface->h};
}


void Surface::SetBlend(SDL_BlendMode mode)
{
	SDL_SetSurfaceBlendMode(surface.get(), mode);
}


void Surface::SetSurface(SDL_Surface *ptr)
{
	surface.reset(ptr);
	UpdateSurface();
}


void Surface::UpdateTexture()
{
	texture = {	SDL_CreateTextureFromSurface(RENDERER, surface.get()), SDL_DestroyTexture };
}

