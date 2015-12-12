//
// Created by Tristan on 12/12/2015.
//

#include "ship.h"


Ship::Ship()
{
	AddPart("core", 0.0f, 0.0f);
	AddPart("scaffold", 0.0f, -64.0f);
	AddPart("pointy", 0.0f, -128.0f);

	AddPart("scaffold", -64.0f, 0.0f);
	AddPart("scaffold", 64.0f, 0.0f);

	AddPart("scaffold", -64.0f, 64.0f);
	AddPart("scaffold", 64.0f, 64.0f);
}


void Ship::AddPart(const std::string &partname, float x, float y)
{
	std::unique_ptr<Part> part = std::unique_ptr<Part>{ new Part{partname, x, y} };
	part_list.push_back(std::move(part));

	InvalidateCursor();
}


void Ship::Render(Camera &cam)
{
	for(auto & part : part_list)
	{
		part->Render(cam, ship_transform);

		if (part_cursor == part.get())
		{
			part->RenderSelected(cam, ship_transform);
		}
	}
}


void Ship::InvalidateCursor()
{
	part_cursor = nullptr;
}


void Ship::SetPartCursor(glm::vec2 cursor)
{
	if (part_list.size() == 0)
	{
		InvalidateCursor();
		return;
	}

	float dist_min = 100.0f;

	for (auto & part : part_list)
	{
		float dist = part->GetDistanceFrom(cursor, ship_transform);
		if (not part_cursor or (dist < dist_min))
		{
			dist_min = dist;
			part_cursor = part.get();
		}
	}

	if (dist_min >= 100.0f) part_cursor = nullptr;
}


