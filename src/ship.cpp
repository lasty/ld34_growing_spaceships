//
// Created by Tristan on 12/12/2015.
//

#include "ship.h"
#include "globals.h"

#include "assets.h"

#include <fstream>
#include <glm/detail/func_geometric.hpp>


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
			part->RenderConnectors(cam, ship_transform, connector_cursor);
		}
	}
}


void Ship::InvalidateCursor()
{
	part_cursor = nullptr;
	connector_cursor = nullptr;
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
		const glm::vec2 this_part_world_pos = ship_transform.GetWorldPosition(part->GetOffset());
		float dist = glm::distance(this_part_world_pos, cursor);

		if (not part_cursor or (dist < dist_min))
		{
			dist_min = dist;
			part_cursor = part.get();
		}
	}

	if (dist_min >= 100.0f) part_cursor = nullptr;
}


void Ship::SetConnectorCursor(glm::vec2 cursor)
{
	connector_cursor = nullptr;
	if (not part_cursor) return;

	float dist_min = 100.0f;

	for (auto & connector : part_cursor->GetConnectors())
	{
		const glm::vec2 connector_world_pos = ship_transform.GetWorldPosition({connector.x, connector.y});
		float dist = glm::distance(cursor, connector_world_pos);

		if (not connector_cursor or (dist < dist_min))
		{
			dist_min = dist;
			connector_cursor = &connector;
		}
	}

	if (dist_min >= 100.0f) connector_cursor = nullptr;

}


void Ship::AttachPartAtCursor(const std::string &partname)
{
	if (not part_cursor) return;
	if (not connector_cursor) return;

	glm::vec2 part_loc = {connector_cursor->x, connector_cursor->y};
	float part_rot = connector_cursor->rot;

	std::unique_ptr<Part> dummy_part = std::unique_ptr<Part>{ new Part{ ASSETS->GetPart(partname), 0.0f, 0.0f, part_rot} };

	Connector &other_connector = dummy_part->GetConnector(0);

	glm::vec2 other_part_loc = {other_connector.x, other_connector.y};

	glm::vec2 new_loc = part_loc - other_part_loc;
	float new_rot = connector_cursor->rot;

	//part_list.push_back(std::move(dummy_part));
	//InvalidateCursor();

	AddPart(partname, new_loc.x, new_loc.y, new_rot);
}

