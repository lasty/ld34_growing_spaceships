//
// Created by Tristan on 8/09/2015.
//

#include "sound.h"


Sound::Sound(const std::string &filename)
: chunk(Mix_LoadWAV(filename.c_str()), &Mix_FreeChunk)
{

}


Sound::~Sound()
{
	//Not sure if this is needed?  Stop all sounds playing before calling Mix_FreeChunk
	Mix_HaltChannel(-1);
}


void Sound::Play() const
{
	Mix_PlayChannel(-1, chunk.get(), 0);
}

void Sound::PlayWithVolume(float volume) const
{
	auto channel = Mix_PlayChannel(-1, chunk.get(), 0);
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;
	int volume_int = (int)(volume * 128);
	Mix_Volume(channel, volume_int);
}
