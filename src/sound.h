
#pragma once


//SDL includes
#include <SDL_mixer.h>

//Standard includes
#include <memory>
#include <string>


class Sound
{
public:
	explicit Sound(const std::string &filename);
	~Sound();

	Sound(Sound &&move_construct) = default;
	Sound & operator=(Sound &&move_assign) = default;

private:
	std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)> chunk;

public:
	void Play() const;
	void PlayWithVolume(float volume) const;

};

