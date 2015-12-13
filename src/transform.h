//
// Created by Tristan on 12/12/2015.
//


#pragma once


#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


class Transform
{
public:
	Transform();

private:
	glm::mat4 transform_matrix;

	glm::vec3 position;
	float rot;

public:

	void SetRotation(float degrees);
	void SetRotationRelative(float degrees);

	float GetRotation() const { return rot; }

	void SetPosition(float x, float y);
	void SetPositionRelative(float x, float y);

	const glm::vec3 & GetPosition() const { return position; };

	void CalcMatrix();

	const glm::mat4 & GetMatrix() const { return transform_matrix; }

	glm::vec2 GetWorldPosition(const glm::vec2 &offset) const;
	float GetWorldRotation(float rot) const;

};


