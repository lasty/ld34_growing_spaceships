//
// Created by Tristan on 12/12/2015.
//

#include "ship.h"
#include "globals.h"

#include "assets.h"

#include <fstream>


Ship::Ship()
{
	AddPart("core", 0.0f, 0.0f, 0.0f);
}


Ship::Ship(const std::string &ship_type)
{
	std::string prefix = DATA_PATH + "ships/";

	std::ifstream in { prefix + ship_type + ".txt" };

	assert(in);


	std::string name;
	in >> name;

	int num_parts;
	in >> num_parts;

	for (int i=0; i < num_parts; i++)
	{
		std::string part_name;
		float x;
		float y;
		float rot;

		in >> part_name >> x >> y >> rot;

		if (in.eof() or in.bad()) break;

		AddPart(part_name, x, y, rot);
	}

}


void Ship::AddPart(const std::string &partname, float x, float y, float rot)
{
	std::unique_ptr<Part> part = std::unique_ptr<Part>{ new Part{ ASSETS->GetPart(partname), x, y, rot} };
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


