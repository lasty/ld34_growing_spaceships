//
// Created by Tristan on 12/12/2015.
//

#include <glm/gtc/matrix_transform.hpp>
#include "part.h"

#include "assets.h"
#include "render.h"

#include <iostream>

Part::Part(std::ifstream &in)
{
	std::string sprite_name;
	in >> sprite_name;
	std::cout << "part sprite name: " << sprite_name << std::endl;

	Sprite &sprite = ASSETS->GetSprite(sprite_name);

	sprite_ref = &sprite;

	int num_collision_circles = 0;
	in >> num_collision_circles;

	std::cout << "num collision circles: " << num_collision_circles << std::endl;


	for(int i=0; i<num_collision_circles; i++)
	{
		float x;
		float y;
		float radius;

		in >> x >> y >> radius;

		if (not in or in.eof() or in.bad()) break;

		collision_circles.push_back(CollisionCircle(x, y, radius));
	}

	int num_connectors = 0;
	in >> num_connectors;

	std::cout << "num connectors: " << num_connectors << std::endl;


	for (int i=0; i< num_connectors; i++)
	{
		float x;
		float y;
		float rot;

		in >> x >> y >> rot;

		if (not in or in.eof() or in.bad()) break;

		connectors.push_back(Connector(x, y, rot));

	}

}


Part::Part(const Part &copy, float x, float y, float rot)
{
	sprite_ref = copy.sprite_ref;
	offset = copy.offset;
	this->rot = copy.rot;
	collision_circles = copy.collision_circles;
	connectors = copy.connectors;

	offset.x = x;
	offset.y = y;
	this->rot = rot;

	for(auto &circle : collision_circles)
	{
		//transform old circle to new circle position

		float rad = glm::radians(this->rot);

		// perform quick 2d rotation
		float newx = (cosf(rad) * circle.x) - (sinf(rad) * circle.y);
		float newy = (cosf(rad) * circle.y) + (sinf(rad) * circle.x);

		// add the old offset
		circle.x = newx + x;
		circle.y = newy + y;
	}


	for(auto &connector : connectors)
	{
		//transform old circle to new circle position

		float rad = glm::radians(this->rot);

		// perform quick 2d rotation
		float newx = (cosf(rad) * connector.x) - (sinf(rad) * connector.y);
		float newy = (cosf(rad) * connector.y) + (sinf(rad) * connector.x);

		// add the old offset
		connector.x = newx + x;
		connector.y = newy + y;
	}

}



void Part::Render(Camera &cam, const Transform &transform)
{
	sprite_ref->Render(cam, offset.x, offset.y, rot, transform);

	//RenderCollisionCircles(cam, transform);

	//RenderConnectors(cam, transform, nullptr);
}


void Part::RenderSelected(Camera &cam, const Transform &transform)
{
	const glm::vec2 this_part_world_pos = transform.GetWorldPosition(offset);

	RenderColour("hud1");
	RenderCircle(cam, this_part_world_pos.x, this_part_world_pos.y, 32);

	RenderConnectors(cam, transform, nullptr);

}


void Part::RenderCollisionCircles(Camera &cam, const Transform &transform)
{
	RenderColour("hud2");

	for(const auto & circle : collision_circles)
	{
		glm::vec2 collision_pos {circle.x, circle.y};

		const glm::vec2 world_pos = transform.GetWorldPosition(collision_pos);

		RenderCircle(cam, world_pos.x, world_pos.y, circle.radius);
	}
}


void Part::RenderConnectors(Camera &cam, const Transform &transform, Connector *selected)
{
	RenderColour("hud3");

	for(const auto & connector : connectors)
	{
		glm::vec2 connector_pos{connector.x, connector.y};

		const glm::vec2 world_pos = transform.GetWorldPosition(connector_pos);

		if (selected == &connector) RenderColour("hud2");

		RenderCircle(cam, world_pos.x, world_pos.y, 16.0f);

		if (selected == &connector) RenderColour("hud3");

	}
}


float Part::GetDistanceFrom(const glm::vec2 &point, const Transform &ship_transform) const
{
	const glm::vec2 this_part_world_pos = ship_transform.GetWorldPosition(offset);

	return glm::distance(this_part_world_pos, point);
}

