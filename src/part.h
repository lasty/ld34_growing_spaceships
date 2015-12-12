//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include "camera.h"
#include "sprite.h"
#include "transform.h"


class Part
{
public:
	Part(const std::string &part_name, float x, float y);

private:

	glm::vec2 offset;
	float rot = 0.0f;

	Sprite * sprite_ref = nullptr;


public:

	void Render(Camera &cam, const Transform & ship_transform);

};


