//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include "sprite.h"
#include "globals.h"
#include "part.h"
#include "font.h"

#include <map>

class Assets
{
public:
	Assets();

private:
	void SetupSurfaces();
	void SetupSprites();
	void SetupFonts();
	void SetupColours();

	void SetupParts();

	std::map<std::string, Surface> surface_list;

	std::map<std::string, Sprite> sprite_list;

	std::map<std::string, Part> part_list;

	std::map<std::string, Font> font_list;

	std::map<std::string, SDL_Color> colour_list;

public:

	Surface & GetSurface(const std::string &name);

	Sprite & GetSprite(const std::string &name);

	Font & GetFont(const std::string &name);

	SDL_Color & GetColour(const std::string &name);

	Part & GetPart(const std::string &name);
};


