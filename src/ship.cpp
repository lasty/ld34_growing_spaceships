//
// Created by Tristan on 12/12/2015.
//

#include "ship.h"
#include "globals.h"

#include "assets.h"

#include "render.h"

#include <fstream>
#include <glm/detail/func_geometric.hpp>

#include <algorithm>
#include <iostream>


Ship::Ship()
{
	//AddPart("core", 0.0f, 0.0f, 0.0f);
}


Ship::Ship(const std::string &ship_type)
{
	std::string prefix = DATA_PATH + "ships/";

	std::ifstream in { prefix + ship_type + ".txt" };

	assert(in);

	Deserialize(in);
}


std::unique_ptr<Ship> Ship::SplitShip()
{
	std::unique_ptr<Ship> new_ship { new Ship() };
	new_ship->GetTransform() = GetTransform();

	for(auto it = part_list.begin(); it != part_list.end(); )
	{
		if ((*it)->GetIsland() == 2)
		{
			new_ship->part_list.push_back(std::move(*it));

			it = part_list.erase(it);
		}
		else
		{
			it++;
		}
	}

	RecalcConnections();
	RecalcCenterOfGravity();

	new_ship->RecalcConnections();
	new_ship->RecalcCenterOfGravity();

	return new_ship;
}



void Ship::Clear()
{
	part_list.clear();
}


bool Ship::ShouldRemove() const
{
	return part_list.size() == 0;
}


void Ship::Deserialize(std::istream &in)
{
	Clear();

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
	RecalcConnections();
	RecalcCenterOfGravity();
}


void Ship::Serialize(std::ostream &out)
{
	out << "custom_ship_name" << "\n";
	out << part_list.size() << "\n";

	for (auto & part : part_list)
	{
		out << part->GetName() << " ";

		const auto & offset = part->GetOffset();
		float rot = part->GetRot();

		out << offset.x << " " << offset.y << " " << rot << "\n";
	}
}


void Ship::AddPart(const std::string &partname, float x, float y, float rot)
{
	std::unique_ptr<Part> part = std::unique_ptr<Part>{ new Part{ ASSETS->GetPart(partname), x, y, rot} };
	part_list.push_back(std::move(part));
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


void Ship::RenderShipSelected(Camera &cam)
{
	RenderColour("hud_selected_part");
	const auto pos = ship_transform.GetPosition();
	const auto rot = ship_transform.GetRotation();
	RenderCircleRotated(cam, pos.x, pos.y, bounding_circle, rot);
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
	RecalcConnections();
	RecalcCenterOfGravity();
}


void Ship::DeletePartAtCursor()
{
	if (not part_cursor) return;

	auto it = std::find_if(part_list.begin(), part_list.end(), [&](auto &p){return p.get() == part_cursor;});
	part_list.erase(it);

	RecalcConnections();
	RecalcCenterOfGravity();
}


void Ship::RecalcConnections()
{
	struct conect_pair { Connector * connector;  Part *part; };

	std::vector<conect_pair> connection_list;

	//flag all parts as not connected, and all connections as not connected

	for(auto &part : part_list)
	{
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

	//reset all parts to no island
	for(auto &part : part_list)
	{
		part->SetIsland(0);
	}

	//start island 1 from the ships core part, if it has one
	if (core)
	{
		island_no++;
		core->SetIslandRecursive(island_no);
	}

	//go through all the other parts, set their island numbers for all connected parts
	for(auto &part : part_list)
	{
		if (part->GetIsland() == 0)
		{
			island_no++;
			part->SetIslandRecursive(island_no);
		}
	}

	needs_splitting = (island_no > 1);

	std::cout << "RecalcIslands: found " << island_no << " islands.  (" << part_list.size() << " parts)" << std::endl;
}


void Ship::RecalcCenterOfGravity()
{
	//Recalculates center of rotation, and bounding circle

	glm::vec2 center;
	glm::vec2 world_center;
	int num = 0;

	for(const auto & part : part_list)
	{
		if (part->GetIsland() == 1)
		{
			center += part->GetOffset();
			world_center += ship_transform.GetWorldPosition(part->GetOffset());
			num++;
		}
	}

	center /= float(num);
	world_center /= float(num);

	//std::cout << "recalculating center, offset is: " << center.x << ", " << center.y << std::endl;

	float max_dist = 1.0f;

	for(auto &part : part_list)
	{
		part->SetOffsetRelative( -center );

		for(auto &circ : part->GetCollisionCircles())
		{
			glm::vec2 circ_pos { circ.x, circ.y };

			float dist = glm::length(circ_pos) + circ.radius;

			max_dist = glm::max(max_dist, dist);
		}
	}

	bounding_circle = max_dist;

	//transfer offset to world position, so the ship parts stay in the same place
	ship_transform.SetPosition(world_center.x , world_center.y);

}
