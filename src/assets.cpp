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


	SetupSurfaces();

	SetupSprites();


	ASSETS = this;
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


Surface &Assets::GetSurface(const std::string &name)
{
	return surface_list.at(name);
}


Sprite &Assets::GetSprite(const std::string &name)
{
	return sprite_list.at(name);
}
