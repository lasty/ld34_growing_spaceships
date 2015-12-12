//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include "sprite.h"
#include "globals.h"

#include <map>

class Assets
{
public:
	Assets();

private:
	void SetupSurfaces();
	void SetupSprites();

	std::map<std::string, Surface> surface_list;

	std::map<std::string, Sprite> sprite_list;

public:

	Surface & GetSurface(const std::string &name);

	Sprite & GetSprite(const std::string &name);

};


