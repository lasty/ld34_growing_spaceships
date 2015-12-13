//
// Created by Tristan on 13/12/2015.
//


#pragma once


#include "text.h"


class HUD
{
public:
	HUD();

private:

	Font &font1;

	Text text1;

public:

	void Update(float dt);

	void Render();

};


