//
// Created by Tristan on 14/12/2015.
//

#include <glm/common.hpp>
#include "tractorbeam.h"
#include "render.h"
#include "globals.h"
#include "game.h"


TractorBeam::TractorBeam(Part *part, glm::vec2 start, glm::vec2 end)
{
	assert(part);

	part_name = part->GetName();

	sprite_ref = part->GetSprite();

	rot = part->GetRot();

	start_pos = start;
	current_pos = start;
	end_pos = end;

	interp = 0.0f;

}


void TractorBeam::Update(float dt)
{
	interp += dt / TRACTOR_SPEED;

	if (interp >= 1.0f)
	{
		interp = 1.0f;
		finished = true;

		GAME->PlayWorldSound("attach", end_pos);
	}

	current_pos = glm::mix(start_pos, end_pos, interp);
}


void TractorBeam::Render(Camera &cam)
{
	sprite_ref->Render(cam, current_pos.x, current_pos.y, rot);

	RenderColour("tractorbeam");
	RenderLine(cam, end_pos.x, end_pos.y, current_pos.x, current_pos.y);

}
