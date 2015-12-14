//
// Created by Tristan on 12/12/2015.
//

#pragma once


#include <vector>
#include <memory>

#include "camera.h"
#include "part.h"
#include "transform.h"
#include "projectile.h"


class Ship
{
public:
	Ship();
	Ship(const std::string &ship_type);

	void Deserialize(std::istream &in);
	void Serialize(std::ostream &out);

	void Clear();

	bool ShouldRemove() const;
	bool ShouldSplit() const { return needs_splitting; }

	std::unique_ptr<Ship> SplitShip();

private:

	std::vector<std::unique_ptr<Part>> part_list;
	Part * core = nullptr;

	bool needs_splitting = false;

	bool enable_clipping = true;

	Transform ship_transform;

	float bounding_circle = 1.0f;

public:

	void AddPart(const std::string &partname, float x, float y, float rot, bool delayed_start);


	void Render(Camera &cam, bool render_connectors, bool render_selected_part);
	void RenderShipSelected(Camera &cam);


	void SetPartCursor(glm::vec2 cursor);
	void SetConnectorCursor(glm::vec2 cursor);

	void AttachPartAtCursor(const std::string &partname, bool delayed_start);

	void DeletePartAtCursor();
	void DeletePartsAroundPart(Part *part);
	void DeletePart(Part *part);

	Part * part_cursor = nullptr;
	Connector * connector_cursor = nullptr;

	void InvalidateCursor();

	Transform & GetTransform() { return ship_transform; }
	const Transform & GetTransform() const { return ship_transform; }

	float GetBoundingCircle() const { return bounding_circle; }

	void RecalcConnections();

	void RecalcIslands();

	void RecalcCenterOfGravity();

	void Update(float dt);

	void CheckCollision(Ship *other, float dt);
	void CheckCollision(Projectile *proj);

	float max_turn_rate = 180.0f;
	float set_heading = 0.0f;
	//float current_heading = 0.0f;
	float current_turn_rate = 0.0f;

	void SetHeading(glm::vec2 point);
	void SetHeading(float deg) { set_heading = deg; }
	void SetHeadingRelative(float deg) { set_heading += deg; }


	glm::vec2 current_velocity;
	glm::vec2 set_velocity;

	void SetThrust(glm::vec2 thrust);

	void UpdateRotation(float dt);

	void UpdatePosition(float dt);

	void ApplyForce(glm::vec2 thrust);


	glm::vec2 GetWorldPosition() const;
	glm::vec2 GetWorldPositionPart(Part *part) const;
	glm::vec2 GetWorldPositionConnection(Connector *conn) const;

	const auto & GetParts() const { return part_list; }

	bool contains_ship_core = false;

	bool IsShip() const { return contains_ship_core; }
	bool IsJunk() const { return not contains_ship_core; }

};


