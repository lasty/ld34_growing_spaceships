//
// Created by Tristan on 13/12/2015.
//


#pragma once


#include "text.h"


class HUD
{
public:
	HUD();

	float width;
	float height;

private:

	Font &font1;
	Font &font_small;

	Text text_help1;


	Text text_control_mode;
	Text text_button1;
	Text text_button2;

	Text text_action1;
	Text text_action2;



public:

	void Resize(int width, int height);

	void Update(float dt);

	void Render();

};


