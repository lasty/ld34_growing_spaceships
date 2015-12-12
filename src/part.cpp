//
// Created by Tristan on 12/12/2015.
//

#include <glm/gtc/matrix_transform.hpp>
#include "part.h"

#include "assets.h"
#include "render.h"


Part::Part(const std::string &part_name, float x, float y)
{
	sprite_ref = &ASSETS->GetSprite(part_name);

	offset.x = x;
	offset.y = y;
	rot = 0.0f;

}


void Part::Render(Camera &cam, const Transform &transform)
{
	sprite_ref->Render(cam, offset.x, offset.y, rot, transform);
}


void Part::RenderSelected(Camera &cam, const Transform &transform)
{
	const glm::vec2 this_part_world_pos = transform.GetWorldPosition(offset);

	RenderColour("hud1");
	RenderCircle(cam, this_part_world_pos.x, this_part_world_pos.y, 32);
}


float Part::GetDistanceFrom(const glm::vec2 &point, const Transform &ship_transform) const
{
	const glm::vec2 this_part_world_pos = ship_transform.GetWorldPosition(offset);

	return glm::distance(this_part_world_pos, point);
}
