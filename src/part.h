//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include "camera.h"
#include "sprite.h"
#include "transform.h"


class Connector
{

};


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

	void RenderSelected(Camera &cam, const Transform & ship_transform);


	float GetDistanceFrom(const glm::vec2 &point, const Transform &ship_transform) const;

};


