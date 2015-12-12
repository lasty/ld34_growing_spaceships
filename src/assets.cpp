//
// Created by Tristan on 12/12/2015.
//

#include "assets.h"

#include "globals.h"


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

	surface_list.emplace("sprite1", Surface{prefix + "sprites1.xcf"});

}

void Assets::SetupSprites()
{

	Surface & sprite1 = GetSurface("sprite1");

	sprite_list.emplace("pointy", Sprite{sprite1, 0, 0, 32, 32, 2});
	sprite_list.emplace("core", Sprite{sprite1, 32, 0, 32, 32, 2});
	sprite_list.emplace("scaffold", Sprite{sprite1, 0, 32, 32, 32, 2});

}


Surface &Assets::GetSurface(const std::string &name)
{
	return surface_list.at(name);
}


Sprite &Assets::GetSprite(const std::string &name)
{
	return sprite_list.at(name);
}
