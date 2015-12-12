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

private:

	std::vector<std::unique_ptr<Part>> part_list;

	Transform ship_transform;

public:

	void AddPart(const std::string &partname, float x, float y);

	void Render(Camera &cam);


	Transform & GetTransform() { return ship_transform; }
	const Transform & GetTransform() const { return ship_transform; }

};


