//
// Created by Tristan on 13/12/2015.
//

#include "text.h"

#include "globals.h"


Text::Text(Font &font, const std::string &text, const SDL_Color &color)
: Surface(nullptr)
, font(font)
, text(text)
, colour(color)
{
	PreRenderText();
}


Text::Text(Font &font, const std::string &text)
: Text(font, text, SDL_Color{255, 255, 255, 255})
{

}


void Text::SetText(const std::string &new_text)
{
	text = new_text;

	PreRenderText();
}


void Text::PreRenderText()
{

	SDL_Surface *surf = TTF_RenderText_Blended(font.GetFont(), text.c_str(), colour);

	SetSurface(surf);  //Parent class Takes ownership

	SetBlend(SDL_BLENDMODE_BLEND);

	UpdateTexture();
}



void Text::Render(Camera &cam, float x, float y)
{
	//TODO
}


void Text::RenderSimple(float x, float y)
{

	SDL_Rect dest = GetRect();

	dest.x = x;
	dest.y = y;

	SDL_RenderCopy(RENDERER, GetTexture(), nullptr, &dest);

}
