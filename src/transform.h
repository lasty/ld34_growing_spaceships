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

	void CalcMatrix();

	const glm::mat4 & GetMatrix() const { return transform_matrix; }

	glm::vec2 TransformPoint(float x, float y) const;

};


