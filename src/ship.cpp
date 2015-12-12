//
// Created by Tristan on 12/12/2015.
//

#include "ship.h"


Ship::Ship()
{
	AddPart("scaffold", 0.0f, 0.0f);
	AddPart("scaffold", 0.0f, -64.0f);

	AddPart("scaffold", -64.0f, 0.0f);
	AddPart("scaffold", 64.0f, 0.0f);

	AddPart("scaffold", -64.0f, 64.0f);
	AddPart("scaffold", 64.0f, 64.0f);
}


void Ship::AddPart(const std::string &partname, float x, float y)
{
	std::unique_ptr<Part> part = std::unique_ptr<Part>{ new Part{partname, x, y} };
	part_list.push_back(std::move(part));
}


void Ship::Render(Camera &cam)
{

	for(auto & part : part_list)
	{
		part->Render(cam, ship_transform);
	}

}
