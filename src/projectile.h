//
// Created by Tristan on 14/12/2015.
//


#pragma once


#include "sprite.h"

class Ship;

class Projectile
{
public:
	Projectile(const std::string &name, glm::vec2 start, glm::vec2 velocity, float radius, float ttl, Ship *ignore_ship);

	Ship * ignore_this_ship = nullptr;

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
	bool IsAlive() const { return alive; }

	void Render(Camera &cam);

	//void CheckCollision();

	const glm::vec2 & GetPosition() const { return position; }
	float GetRadius() const { return radius; }

	void SetRemove() { alive = false; }

};


