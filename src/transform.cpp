//
// Created by Tristan on 12/12/2015.
//

#include <glm/gtc/matrix_transform.hpp>
#include "transform.h"


Transform::Transform()
{
	CalcMatrix();
}

void Transform::SetRotation(float degrees)
{
	rot = degrees;

	CalcMatrix();
}

void Transform::SetRotationRelative(float degrees)
{
	rot += degrees;

	SetRotation(rot);
}


void Transform::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;

	CalcMatrix();
}


void Transform::SetPositionRelative(float x, float y)
{
	position.x += x;
	position.y += y;

	SetPosition(position.x, position.y);
}


void Transform::CalcMatrix()
{
	glm::mat4 translate_matrix = glm::translate( glm::mat4{}, glm::vec3{position, 0.0f} );
	glm::mat4 rot_matrix = glm::rotate( glm::mat4{}, glm::radians(rot), glm::vec3{0.0f, 0.0f, 1.0f} );

	transform_matrix = translate_matrix * rot_matrix;
	//transform_matrix = rot_matrix * translate_matrix;
}


glm::vec2 Transform::GetWorldPosition(const glm::vec2 &offset) const
{
	glm::vec4 p { offset.x, offset.y, 0.0f, 1.0f};

	p = transform_matrix * p;

	return {p.x, p.y};
}

float Transform::GetWorldRotation(float rot) const
{
	return rot + this->rot;
}
