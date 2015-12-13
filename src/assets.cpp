//
// Created by Tristan on 12/12/2015.
//

#include "assets.h"

#include "globals.h"

#include <fstream>
#include <iostream>


Assets::Assets()
{
	assert(RENDERER);

	assert(ASSETS == nullptr);

	//Early Setup

	SetupSurfaces();

	SetupSprites();

	SetupFonts();

	SetupColours();

	ASSETS = this;

	//Deferred Setup  (Requires ASSETS to be set)

	SetupParts();

}


void Assets::SetupSurfaces()
{
	std::string prefix = DATA_PATH + "images/";
	//std::string data_file = DATA_PATH + "images/";

	std::ifstream in{ prefix + "images.txt" };
	assert(in);

	while (in and in.good())
	{
		std::string name;
		std::string filename;

		in >> name;
		in >> filename;

		if (in.eof()) break;

		std::cout << "name: " << name << "  filename: " << filename << std::endl;

		surface_list.emplace(name, Surface{prefix + filename});
	}

}

void Assets::SetupSprites()
{
	std::ifstream in{ DATA_PATH + "sprites.txt" };
	assert(in);

	while (in and in.good())
	{
		std::string name;
		std::string spritesheet;
		int x;
		int y;
		int w;
		int h;
		float zoom;

		in >> name >> spritesheet >> x >> y >> w >> h >> zoom;

		if (in.eof()) break;

		std::cout << "name: " << name << std::endl;

		Surface & sprite_surf = GetSurface(spritesheet);

		sprite_list.emplace(name, Sprite{sprite_surf, x, y, w, h, zoom});

	}
	//Surface & sprite1 = GetSurface("sprites1");

	//sprite_list.emplace("pointy", Sprite{sprite1, 0, 0, 32, 32, 2});
	//sprite_list.emplace("core", Sprite{sprite1, 32, 0, 32, 32, 2});
	//sprite_list.emplace("scaffold", Sprite{sprite1, 0, 32, 32, 32, 2});

}


void Assets::SetupFonts()
{
	std::string prefix = DATA_PATH + "fonts/";

	font_list.emplace("mono", Font{prefix+"Roboto_Mono/RobotoMono-Regular.ttf", 32});

	font_list.emplace("small", Font{prefix+"Roboto_Slab/RobotoSlab-Regular.ttf", 16});

	font_list.emplace("hud", Font{prefix+"Roboto_Slab/RobotoSlab-Regular.ttf", 32});

}


void Assets::SetupColours()
{
	colour_list.emplace("background", SDL_Color{10, 20, 30, 255});
	colour_list.emplace("hud_background_sep", SDL_Color{200, 200, 192, 200});
	colour_list.emplace("hud_background0", SDL_Color{128, 128, 128, 240});
	colour_list.emplace("hud_background1", SDL_Color{237, 167, 31, 240});
	colour_list.emplace("hud_background2", SDL_Color{184, 45, 45, 240});

	colour_list.emplace("blue", SDL_Color{128, 128, 255, 255});
	colour_list.emplace("red", SDL_Color{255, 32, 32, 255});
	colour_list.emplace("grey", SDL_Color{192, 192, 192, 255});
	colour_list.emplace("white", SDL_Color{255, 255, 255, 255});
	colour_list.emplace("green", SDL_Color{32, 255, 32, 255});
}


void Assets::SetupParts()
{
	std::ifstream in{DATA_PATH + "parts.txt"};
	assert(in);

	while (in and in.good())
	{
		std::string name;

		in >> name;

		if (in.bad() or in.eof() or name.empty()) break;
		std::cout << "part name: " << name << std::endl;

		part_list.emplace(name, Part{name, in});
	}
}




Surface &Assets::GetSurface(const std::string &name)
{
	return surface_list.at(name);
}


Sprite &Assets::GetSprite(const std::string &name)
{
	return sprite_list.at(name);
}


Font &Assets::GetFont(const std::string &name)
{
	return font_list.at(name);
}


SDL_Color & Assets::GetColour(const std::string &name)
{
	return colour_list.at(name);
}


Part &Assets::GetPart(const std::string &name)
{
	return part_list.at(name);
}
