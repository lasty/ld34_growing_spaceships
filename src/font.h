//
// Created by Tristan on 13/12/2015.
//


#pragma once

#include <SDL_ttf.h>

#include <memory>

class Font
{
public:
	Font(const std::string &filename, int size);

private:

	std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font_ptr;

public:

	TTF_Font *GetFont() const { return font_ptr.get(); };
};


