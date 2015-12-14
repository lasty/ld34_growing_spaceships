//
// Created by Tristan on 14/12/2015.
//


#pragma once


#include "sprite.h"

class Projectile
{
public:
	Projectile(const std::string &name, glm::vec2 start, glm::vec2 velocity, float radius, float ttl);

private:

	Sprite *sprite_ref = nullptr;

	glm::vec2 position;
	glm::vec2 velocity;

	float radius = 32.0f;

	bool alive = true;
	float ttl = 1.0f;
	float rot = 0.0f;

public:

	void Update(float dt);

	bool ShouldRemove() const { return not alive; }

	void Render(Camera &cam);

	void CheckCollision();
};


