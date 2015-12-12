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

private:

	std::vector<std::unique_ptr<Part>> part_list;

	Transform ship_transform;

public:

	void AddPart(const std::string &partname, float x, float y, float rot);


	void Render(Camera &cam);


	void SetPartCursor(glm::vec2 cursor);
	void SetConnectorCursor(glm::vec2 cursor);

	void AttachPartAtCursor(const std::string &partname);
	void DeletePartAtCursor();

	Part * part_cursor = nullptr;
	Connector * connector_cursor = nullptr;

	void InvalidateCursor();

	Transform & GetTransform() { return ship_transform; }
	const Transform & GetTransform() const { return ship_transform; }


	void RecalcConnections();

	void RecalcIslands();

};


