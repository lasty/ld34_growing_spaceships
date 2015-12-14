//
// Created by Tristan on 14/12/2015.
//

#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include "projectile.h"

#include "globals.h"
#include "assets.h"


Projectile::Projectile(const std::string &name, glm::vec2 start, glm::vec2 velocity, float radius, float ttl)
{
	sprite_ref = &ASSETS->GetSprite(name);

	this->position = start;

	float linear_vel = 1000.0f;
	if (name == "laser")  linear_vel = 2000.0f;
	velocity = glm::normalize(velocity) * linear_vel;
	this->velocity = velocity;

	this->radius = radius;

	this->ttl = ttl;

	rot = glm::degrees(atan2f(velocity.y, velocity.x))+90.0f;

}


void Projectile::Update(float dt)
{
	ttl -= dt;
	if (ttl <= 0.0f) alive = false;

	position += velocity * dt;
}


void Projectile::Render(Camera &cam)
{
	sprite_ref->Render(cam, position.x, position.y, rot);
}

