//
// Created by Tristan on 14/12/2015.
//

#include <glm/detail/func_trigonometric.hpp>
#include "projectile.h"

#include "globals.h"
#include "assets.h"


Projectile::Projectile(const std::string &name, glm::vec2 start, glm::vec2 velocity, float radius, float ttl)
{
	sprite_ref = &ASSETS->GetSprite(name);

	this->position = start;
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

