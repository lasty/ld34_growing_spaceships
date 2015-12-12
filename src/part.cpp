//
// Created by Tristan on 12/12/2015.
//

#include <glm/gtc/matrix_transform.hpp>
#include "part.h"

#include "assets.h"


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

