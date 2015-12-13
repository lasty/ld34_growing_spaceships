//
// Created by Tristan on 13/12/2015.
//

#include "hud.h"


#include "assets.h"
#include "render.h"

#include "globals.h"


HUD::HUD()
: font1(ASSETS->GetFont("hud"))
, font_small(ASSETS->GetFont("small"))

, text_help1(font_small, "Use WASD/Arrows to move, TAB/SPACE: Change mode", ASSETS->GetColour("white"))

, text_control_mode(font1, "MODE: Scavenge")
, text_button1(font_small, "Left Click:")
, text_button2(font_small, "Right Click:")

, text_action1(font1, "ATTACH")
, text_action2(font1, "SCRAP")
{

}


void HUD::Resize(int width, int height)
{
	this->width = width;
	this->height = height;
}


void HUD::Update(float dt)
{

}


void HUD::Render()
{

	SDL_Rect rect { 0, int(height) - 100, int(width), 100};
	RenderColour("hud_background");

	SDL_RenderFillRect(RENDERER, &rect);

	rect.y -= 1;
	rect.h = 3;
	RenderColour("hud_background_sep");

	SDL_RenderFillRect(RENDERER, &rect);

	text_help1.RenderSimple(10, height - 10 - text_help1.GetHeight());

	float col0 = width - 700;
	float row0 = height - 70;

	float col1 = width - 350;
	float col2 = width - 150;

	float row1 = height - 90;
	float row2 = height - 50;

	text_control_mode.RenderSimple(col0, row0);

	text_button1.RenderSimple(col1, row1);
	text_action1.RenderSimple(col1, row2);

	text_button2.RenderSimple(col2, row1);
	text_action2.RenderSimple(col2, row2);

}
