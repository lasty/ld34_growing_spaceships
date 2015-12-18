
#pragma once

//Wrapper around SDL surface class, and render texture

// SDL includes
#include <SDL_surface.h>
#include <SDL_render.h>

// Standard includes
#include <memory>
#include <assert.h>


class Surface
{
public:
	Surface(int width, int height);
	explicit Surface(const std::string &filename);
	explicit Surface(SDL_Surface *ptr);

	Surface(const Surface &copy) = delete;
	Surface(Surface &&move) = default;

private:
	std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface;
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture;

	SDL_Rect surface_rect;
	SDL_BlendMode blend_mode = SDL_BLENDMODE_NONE;


public:
	void SetBlend(SDL_BlendMode mode);

	void Lock();
	void Unlock();

	void UpdateSurface();
	void UpdateTexture();

protected:
	void SetSurface(SDL_Surface *ptr);


public:
	SDL_Surface * GetSurface() const { return surface.get(); }
	SDL_Texture * GetTexture() const { return texture.get(); }

	const SDL_Rect & GetRect() const { return surface_rect; }

	int GetWidth() const { return GetRect().w; }
	int GetHeight() const { return GetRect().h; }

	//Renderer & GetRenderer() { return renderer; }
};


