//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include "camera.h"
#include "sprite.h"
#include "transform.h"

#include <fstream>
#include <vector>

class Connector
{

};


class CollisionCircle
{
public:
	CollisionCircle(float x, float y, float radius)
	: x{x}, y{y}, radius{radius}
	{ }

public:
	float x = 0.0f;
	float y = 0.0f;
	float radius = 0.0f;
};



class Part
{
public:
	Part(std::ifstream &in);

	Part(const Part &copy, float x, float y);


private:

	glm::vec2 offset;
	float rot = 0.0f;

	Sprite * sprite_ref = nullptr;

	std::vector<CollisionCircle> collision_circles;

public:

	void Render(Camera &cam, const Transform & ship_transform);

	void RenderSelected(Camera &cam, const Transform & ship_transform);

	void RenderCollisionCircles(Camera &cam, const Transform &transform);


	float GetDistanceFrom(const glm::vec2 &point, const Transform &ship_transform) const;

};


