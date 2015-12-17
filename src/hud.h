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
	Font &font_med;
	Font &font_small;

	Text text_help1;


	Text text_control_mode;
	Text text_button1;
	Text text_button2;

	Text text_action1;
	Text text_action2;

	bool tutorial_enabled = false;
	Text tutorial0;
	Text tutorial1;
	Text tutorial2;

	Text ships_nearby;
	Text junk_nearby;


	Text Title_1;
	Text Title_2;

	Text Menu_Choose_Ship;
	Text Menu_Tutorial;
	Text Menu_Exit;


public:

	void Resize(int width, int height);

	void Update(float dt);

	void Render();

	void SetMode(Mode mode);

	void UpdateShipCount(int ships, int junk);

	void SetTutorial(int n, int m, const std::string &text1, const std::string text2);

	void RenderTutorial();

	void RenderMainMenu();

	int GetHudSize() const;
};


