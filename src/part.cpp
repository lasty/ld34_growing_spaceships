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
}


Part::Part(const Part &copy, float x, float y)
{
	sprite_ref = copy.sprite_ref;
	offset = copy.offset;
	rot = copy.rot;
	collision_circles = copy.collision_circles;

	offset.x = x;
	offset.y = y;

	for(auto &circle : collision_circles)
	{
		circle.x += x;
		circle.y += y;
	}

}



void Part::Render(Camera &cam, const Transform &transform)
{
	sprite_ref->Render(cam, offset.x, offset.y, rot, transform);

	RenderCollisionCircles(cam, transform);
}


void Part::RenderSelected(Camera &cam, const Transform &transform)
{
	const glm::vec2 this_part_world_pos = transform.GetWorldPosition(offset);

	RenderColour("hud1");
	RenderCircle(cam, this_part_world_pos.x, this_part_world_pos.y, 32);
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


float Part::GetDistanceFrom(const glm::vec2 &point, const Transform &ship_transform) const
{
	const glm::vec2 this_part_world_pos = ship_transform.GetWorldPosition(offset);

	return glm::distance(this_part_world_pos, point);
}
