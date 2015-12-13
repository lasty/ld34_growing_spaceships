//
// Created by Tristan on 12/12/2015.
//

#pragma once


#include <vector>
#include <memory>

#include "camera.h"
#include "part.h"
#include "transform.h"


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


	Transform ship_transform;

	float bounding_circle = 1.0f;

public:

	void AddPart(const std::string &partname, float x, float y, float rot);


	void Render(Camera &cam);
	void RenderShipSelected(Camera &cam);


	void SetPartCursor(glm::vec2 cursor);
	void SetConnectorCursor(glm::vec2 cursor);

	void AttachPartAtCursor(const std::string &partname);
	void DeletePartAtCursor();

	Part * part_cursor = nullptr;
	Connector * connector_cursor = nullptr;

	void InvalidateCursor();

	Transform & GetTransform() { return ship_transform; }
	const Transform & GetTransform() const { return ship_transform; }

	float GetBoundingCircle() const { return bounding_circle; }

	void RecalcConnections();

	void RecalcIslands();

	void RecalcCenterOfGravity();

};


