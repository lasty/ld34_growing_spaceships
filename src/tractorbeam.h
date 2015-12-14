//
// Created by Tristan on 14/12/2015.
//


#pragma once

#include <string>
#include "sprite.h"
#include "part.h"
#include "ship.h"
#include <memory>


class TractorBeam
{
public:
	TractorBeam(Part *part, glm::vec2 start, glm::vec2 end);

	std::string part_name;

	Sprite * sprite_ref = nullptr;

	glm::vec2 start_pos;
	glm::vec2 end_pos;
	glm::vec2 current_pos;
	float rot;

	float interp;

	bool finished = false;

public:

	void Update(float dt);

	void Render(Camera &cam);


};


