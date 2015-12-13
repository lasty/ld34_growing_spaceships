//
// Created by Tristan on 13/12/2015.
//


#pragma once

#include "surface.h"
#include "font.h"
#include "camera.h"

class Text : public Surface
{
public:
	Text(Font &font, const std::string &text, const SDL_Color &colour);
	Text(Font &font, const std::string &text);

private:
	Font &font;
	std::string text;
	SDL_Color colour;

	void PreRenderText();

public:

	void SetText(const std::string &new_text);

	void Render(Camera &cam, float x, float y);
	void RenderSimple(float x, float y);


};


