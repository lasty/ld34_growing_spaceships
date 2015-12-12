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
public:
	Connector(float x, float y, float rot)
	: x{x}, y{y}, rot{rot}
	{ }

	float x = 0.0f;
	float y = 0.0f;
	float rot = 0.0f;
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

	Part(const Part &copy, float x, float y, float rot);


private:

	glm::vec2 offset;
	float rot = 0.0f;

	Sprite * sprite_ref = nullptr;

	std::vector<CollisionCircle> collision_circles;

	std::vector<Connector> connectors;

public:

	void Render(Camera &cam, const Transform & ship_transform);

	void RenderSelected(Camera &cam, const Transform & ship_transform);

	void RenderCollisionCircles(Camera &cam, const Transform &transform);
	void RenderConnectors(Camera &cam, const Transform &transform, Connector *selected);

	std::vector<Connector> & GetConnectors() { return connectors; }
	Connector & GetConnector(int i) { return connectors.at(i); }

	const glm::vec2 & GetOffset() const { return offset; }

	void SetOffset(const glm::vec2 &new_offset) { offset = new_offset; }

	float GetRot() const { return rot; }

};


