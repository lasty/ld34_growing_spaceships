//
// Created by Tristan on 13/12/2015.
//


#pragma once


#include "text.h"
#include "globals.h"


class HUD
{
public:
	HUD();

	float width;
	float height;

private:
	SDL_Color bgcol;

	Font &font1;
	Font &font_small;

	Text text_help1;


	Text text_control_mode;
	Text text_button1;
	Text text_button2;

	Text text_action1;
	Text text_action2;

	Text warning_select_part_first;
	Text warning_select_part_join;

	Text ships_nearby;
	Text junk_nearby;

	//Text text_game_over;
	//Text text_game_over2;

public:

	void Resize(int width, int height);

	void Update(float dt);

	void Render();

	void SetMode(Mode mode);

	void RenderWarning_SelectPartFirst(SDL_Point pos);
	void RenderWarning_SelectPartJoin(SDL_Point pos);

	void UpdateShipCount(int ships, int junk);

	//void RenderGameOver1();
	//void RenderGameOver2();

};


