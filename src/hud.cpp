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

, text_control_mode(font1, "MODE: ...")
, text_button1(font_small, "Left Click:")
, text_button2(font_small, "Right Click:")

, text_action1(font1, "...")
, text_action2(font1, "...")


, warning_select_part_first(font_small, "Step 1: Select A Destroyed Ship Part To Attach", ASSETS->GetColour("hud_background0"))
, warning_select_part_join(font_small, "Step 2: Select Attachment Point", ASSETS->GetColour("hud_background0"))

{
	bgcol = ASSETS->GetColour("hud_background0");

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
	RenderColour(bgcol);

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

void HUD::SetMode(Mode mode)
{
	if (mode == Mode::Scavenge)
	{
		bgcol = ASSETS->GetColour("hud_background1");

		text_control_mode.SetText("MODE: Scavenge");
		text_action1.SetText("ATTACH");
		text_action2.SetText("SCRAP");
	}

	if (mode == Mode::Combat)
	{
		bgcol = ASSETS->GetColour("hud_background2");

		text_control_mode.SetText("MODE: Combat!");
		text_action1.SetText("LASER");
		text_action2.SetText("MISSILE");
	}
}

void HUD::RenderWarning_SelectPartFirst(SDL_Point pos)
{
	pos.x -= warning_select_part_first.GetWidth() / 2;
	//pos.y -= warning_select_part_first.GetHeight();

	warning_select_part_first.RenderSimple(pos.x, pos.y);
}


void HUD::RenderWarning_SelectPartJoin(SDL_Point pos)
{
	pos.x -= warning_select_part_join.GetWidth() / 2;
	//pos.y -= warning_select_part_first.GetHeight();

	warning_select_part_join.RenderSimple(pos.x, pos.y);
}
