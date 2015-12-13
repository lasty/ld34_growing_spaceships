//
// Created by Tristan on 13/12/2015.
//

#include "font.h"


Font::Font(const std::string &filename, int size)
: font_ptr(TTF_OpenFont(filename.c_str(), size), TTF_CloseFont)
{
	TTF_SetFontHinting(font_ptr.get(), TTF_HINTING_MONO);
}

