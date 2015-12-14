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

	bool is_connected = false;
	class Part * connects_to = nullptr;
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
	Part(const std::string &name, std::ifstream &in);

	Part(const Part &copy, float x, float y, float rot);


private:

	int island = 0;
	std::string part_name;

	float invisible_timer = 0.0f;

	glm::vec2 offset;
	float rot = 0.0f;

	Sprite * sprite_ref = nullptr;

	std::vector<CollisionCircle> collision_circles;

	std::vector<Connector> connectors;

public:

	void Update(float dt);
	void SetInvisibleTimer(float t) { invisible_timer = t; }

	void Render(Camera &cam, const Transform & ship_transform);

	void RenderSelected(Camera &cam, const Transform & ship_transform);

	void RenderCollisionCircles(Camera &cam, const Transform &transform);
	void RenderConnectors(Camera &cam, const Transform &transform, Connector *selected);

	std::vector<Connector> & GetConnectors() { return connectors; }
	Connector & GetConnector(int i) { return connectors.at(i); }

	std::vector<CollisionCircle> & GetCollisionCircles() { return collision_circles; }

	const std::string & GetName() const { return part_name; }
	const glm::vec2 & GetOffset() const { return offset; }

	void SetOffsetRelative(const glm::vec2 &rel_offset);

	float GetRot() const { return rot; }

	Sprite * GetSprite() const { return sprite_ref; }

	void SetIsland(int new_island) { island = new_island; }
	int GetIsland() const { return island; }

	void SetIslandRecursive(int island_no);

};


