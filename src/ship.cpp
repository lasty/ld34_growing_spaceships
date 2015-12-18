//
// Created by Tristan on 12/12/2015.
//

#include "ship.h"
#include "globals.h"

#include "assets.h"

#include "render.h"

#include <fstream>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/random.hpp>

#include <algorithm>
//#include <iostream>

#include "game.h"

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
	new_ship->SetHeadingRelative((rand() % 360) - 180);

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
	InvalidateCursor();
	part_list.clear();
}


bool Ship::ShouldRemove() const
{
	return part_list.size() == 0;
}


void Ship::Load(const std::string &ship_type)
{
	Clear();
	std::ifstream in{DATA_PATH+"/ships/"+ship_type+".txt"};
	Deserialize(in);
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

		AddPart(part_name, x, y, rot, false);
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


void Ship::AddPart(const std::string &partname, float x, float y, float rot, bool delayed_start)
{
	std::unique_ptr<Part> part = std::unique_ptr<Part>{ new Part{ ASSETS->GetPart(partname), x, y, rot} };
	if (delayed_start)
	{
		part->SetInvisibleTimer(TRACTOR_SPEED + 0.015f);
	}

	part_list.push_back(std::move(part));
}


void Ship::Render(Camera &cam, bool render_connectors, bool render_selected_part)
{
	for(auto & part : part_list)
	{
		part->Render(cam, ship_transform);

		if (part_cursor == part.get())
		{
			if (render_selected_part) { part->RenderSelected(cam, ship_transform); }
			if (render_connectors) { part->RenderConnectors(cam, ship_transform, connector_cursor); }
		}
	}
}


void Ship::RenderShipSelected(Camera &cam)
{
	RenderColour("blue");
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


void Ship::AttachPartAtCursor(const std::string &partname, bool delayed_start)
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

	AddPart(partname, new_loc.x, new_loc.y, new_rot, delayed_start);
	RecalcConnections();
	RecalcCenterOfGravity();
}


void Ship::DetachPartAtCursor()
{
	if (not part_cursor)
	{
		GAME->PlayWorldSound("error", GetWorldPosition());
		return;
	}

	if (part_cursor->GetName() == "core") //Don't delete the core
	{
		GAME->PlayWorldSound("error", GetWorldPosition());
		return;
	}

	GAME->PlayWorldSound("attach", GetWorldPosition());
	part_cursor->DisconnectAll();

	RecalcIslands();
	RecalcCenterOfGravity();
}


void Ship::DeletePart(Part *part)
{
	if (not part) return;

	auto it = std::find_if(part_list.begin(), part_list.end(), [&](auto &p){return p.get() == part;});
	part_list.erase(it);
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

	//std::cout << "RecalcIslands: found " << island_no << " islands.  (" << part_list.size() << " parts)" << std::endl;
}


void Ship::RecalcCenterOfGravity()
{
	//Recalculates center of rotation, and bounding circle
	//calculates if ship has a core or not (if not it will be marked as space junk)

	glm::vec2 center;
	glm::vec2 world_center;
	int num = 0;

	contains_ship_core = false;
	num_lasers = 0;
	num_missiles = 0;

	for(const auto & part : part_list)
	{
		if (part->GetIsland() == 1)
		{
			center += part->GetOffset();
			world_center += ship_transform.GetWorldPosition(part->GetOffset());
			num++;

			if (part->GetName() == "core") contains_ship_core = true;

			if (part->GetName() == "laser") num_lasers++;
			if (part->GetName() == "launcher") num_missiles++;
		}
	}

	if (num == 0) return;

	center /= float(num);
	world_center /= float(num);

	//std::cout << "recalculating center, offset is: " << center.x << ", " << center.y << std::endl;


	for(auto &part : part_list)
	{
		part->SetOffsetRelative( -center );
	}

	//recalc bounding circle
	float max_dist = 32.0f;

	for(auto &part : part_list)
	{
		float dist = glm::distance(center, part->GetOffset()) + 32.0f;

		max_dist = glm::max(dist, max_dist);
	}

	bounding_circle = max_dist;

	//transfer offset to world position, so the ship parts stay in the same place
	ship_transform.SetPosition(world_center.x , world_center.y);

}


void Ship::Update(float dt)
{
	UpdateRotation(dt);

	UpdatePosition(dt);

	for(auto &part : part_list)
	{
		part->Update(dt);
	}
}


void Ship::CheckCollision(Ship *other, float dt)
{
	if (not(enable_clipping and other->enable_clipping)) return;

	const glm::vec2 &pos1 = ship_transform.GetPosition();
	const glm::vec2 &pos2 = other->GetTransform().GetPosition();

	float distance = glm::distance(pos1, pos2);

	float radius1 = bounding_circle;
	float radius2 = other->bounding_circle;

	if (distance < (radius1 + radius2))
	{
		//nudge the ships apart

		glm::vec2 angle = pos2 - pos1;

		//glm::vec2 force = -angle * 2.0f * dt;
		glm::vec2 force = -angle * 0.00025f * dt;

		ApplyForce(force);
		other->ApplyForce(-force);
		//ship_transform.SetPositionRelative(force.x, force.y);
		//other->GetTransform().SetPositionRelative(-force.x, -force.y);
	}

}


void Ship::CheckCollision(Projectile *proj)
{
	assert(proj);
	if (proj->ignore_this_ship == this) return;

	//if (not proj->IsAlive()) return;

	const glm::vec2 &pos1 = ship_transform.GetPosition();
	const glm::vec2 &pos2 = proj->GetPosition();

	float distance = glm::distance(pos1, pos2);

	float radius1 = bounding_circle;
	float radius2 = proj->GetRadius();

	//broad phase collision
	if (distance < (radius1 + radius2))
	{
		//check if any parts are hit

		for(auto &part : part_list)
		{
			glm::vec2 pos3 = GetWorldPositionPart(part.get());
			float radius3 = 32.0f;  //XXX Hard coded part radius

			float distance2 = glm::distance(pos2, pos3);

			if (distance2 < (radius2 + radius3))
			{
				//This part is hit

				if (part->GetName() == "core")
				{
					GAME->PlayWorldSound("explosion2", pos3);

					//set thrust to 0, or ships will keep drifting in last set velocity
					SetThrust({0.0, 0.0});
				}
				else
				{
					GAME->PlayWorldSound("explosion1", pos3);
				}


				//Apply damage to part
				//TODO: refactor this, and/or add hitpoints

				if (part->GetName() == "armour")
				{
					part->ChangeType("scaffold");
				}
				else
				{
					DeletePart(part.get());
				}

				proj->SetRemove();

				RecalcConnections();
				RecalcCenterOfGravity();
				return;
			}
		}
	}

}


void Ship::SetHeading(glm::vec2 point)
{
	glm::vec2 pos = ship_transform.GetPosition();

	float angle = glm::degrees(atan2f(point.y - pos.y, point.x - pos.x)) + 90;

	set_heading = angle;

}


void Ship::UpdateRotation(float dt)
{
	float current_heading = ship_transform.GetRotation();

	glm::vec2 current_dir { cosf(glm::radians(current_heading)) , sinf(glm::radians(current_heading))};;
	current_dir = glm::normalize(current_dir);

	glm::vec2 desired_dir { cosf(glm::radians(set_heading)) , sinf(glm::radians(set_heading))};;

	current_dir += (desired_dir - current_dir) * glm::radians(max_turn_rate) * dt;

	float new_angle = atan2f(current_dir.y, current_dir.x);


	current_heading = glm::degrees(new_angle);

	ship_transform.SetRotation(current_heading);

}


void Ship::SetThrust(glm::vec2 thrust)
{
	set_velocity = thrust;
}


void Ship::ApplyForce(glm::vec2 thrust)
{
	current_velocity += thrust;

	///ship_transform.SetPositionRelative(thrust.x, thrust.y);

}


void Ship::UpdatePosition(float dt)
{
	current_velocity += (set_velocity - current_velocity) * dt * 2.0f;
	//current_velocity = set_velocity;

	ship_transform.SetPositionRelative(current_velocity.x * 1000.0f * dt, current_velocity.y * 1000.0f * dt);
}


glm::vec2 Ship::GetWorldPosition() const
{
	return ship_transform.GetWorldPosition(glm::vec2{0.0f, 0.0f});
}


glm::vec2 Ship::GetWorldPositionPart(Part *part) const
{
	assert(part);
	return ship_transform.GetWorldPosition(part->GetOffset());
}


glm::vec2 Ship::GetWorldPositionConnection(Connector *conn) const
{
	assert(conn);

	glm::vec2 pos { conn->x, conn->y};

	return ship_transform.GetWorldPosition(pos);
}


int Ship::CountNumParts(const std::string &name) const
{
	int num = 0;
	for(auto &part : part_list)
	{
		if (part->GetName() == name) num++;
	}
	return num;
}

float rand_time(float min, float max)
{
	float range = max-min;
	return (rand()%100/100.0f) * range + min;
}


bool rand_bool()
{
	return (rand() %2 == 0);
}

float random_rotation()
{
	return (rand()%360);
}

void Ship::EnemyShipAI(float dt, const glm::vec2 &player_pos)
{
	ai_timer_move -= dt;
	if (ai_timer_move <= 0.0f)
	{
		ai_timer_move = rand_time(0.5f, 2.0f);

		if (rand_bool())
		{
			//stop
			SetThrust({0.0f, 0.0f});
		}
		else
		if (rand_bool())
		{
			//move towards player
			glm::vec2 dir = player_pos - GetWorldPosition();
			dir = glm::normalize(dir) * 0.5f;

			SetThrust(dir);
		}
		else
		{
			//move to random location
			glm::vec2 rand = glm::diskRand(0.5f);
			SetThrust(rand);
		}
	}

	ai_timer_rotate -= dt;
	if (ai_timer_rotate <= 0.0f)
	{
		ai_timer_rotate = rand_time(0.5f, 2.0f);

		if (rand_bool())
		{
			//set heading towards player
			SetHeading(player_pos);
		}
		else
		{
			//set random heading
			SetHeading(random_rotation());
		}
	}


	ai_timer_shoot -= dt;
	if (ai_timer_shoot <= 0.0f)
	{
		ai_timer_shoot = rand_time(1.0f, 3.0f);

		if (rand_bool())
		{
			//shoot group 1
			if (GetNumLasers())
			{
				GAME->FireWeapons(*this, 1, player_pos);
			}
		}
		else
		{
			//shoot group 2
			if (GetNumMissiles())
			{
				GAME->FireWeapons(*this, 2, player_pos);
			}
		}

	}

}
