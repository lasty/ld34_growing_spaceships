//
// Created by Tristan on 12/12/2015.
//

#include "ship.h"
#include "globals.h"

#include "assets.h"

#include <fstream>
#include <glm/detail/func_geometric.hpp>

#include <algorithm>

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

	RecalcConnections();

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

	if (connector_cursor->is_connected) return;


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


void Ship::DeletePartAtCursor()
{
	if (not part_cursor) return;

	auto it = std::find_if(part_list.begin(), part_list.end(), [&](auto &p){return p.get() == part_cursor;});
	part_list.erase(it);

	RecalcConnections();
}


void Ship::RecalcConnections()
{
	struct conect_pair { Connector * connector;  Part *part; };

	std::vector<conect_pair> connection_list;

	//flag all parts as not connected, and all connections as not connected

	for(auto &part : part_list)
	{
		part->SetIsland(0);

		for(auto &connector : part->GetConnectors())
		{
			connector.is_connected = false;
			connection_list.push_back({&connector, part.get()});
		}
	}

	for(auto &pair1 : connection_list)
	{
		for(auto &pair2 : connection_list)
		{
			if (&pair1 == &pair2) continue;
			if (pair1.connector == pair2.connector) continue;
			if (pair1.connector->is_connected or pair2.connector->is_connected) continue;

			glm::vec2 pos1 {pair1.connector->x, pair1.connector->y};
			glm::vec2 pos2 {pair2.connector->x, pair2.connector->y};

			float dist = glm::distance(pos1, pos2);

			if (dist < 1.0f)
			{
				pair1.connector->is_connected = true;
				pair1.connector->connects_to = pair2.part;

				pair2.connector->is_connected = true;
				pair2.connector->connects_to = pair1.part;
			}
		}
	}

	RecalcIslands();
}

void Ship::RecalcIslands()
{
	int island_no = 0;

	for(auto &part : part_list)
	{
		island_no++;

		if (part->GetIsland() == 0)
		{
			part->SetIslandRecursive(island_no);
		}
	}
}
