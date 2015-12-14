//
// Created by Tristan on 12/12/2015.
//


#pragma once

#include <SDL_rect.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


struct Rect
{
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
};



class Camera
{
public:
	Camera();
	//Camera(int w, int h);

private:
	glm::vec2 offset;
	float zoom = 1.0f;

	glm::mat4 camera_matrix;
	glm::mat4 projection_matrix;
	glm::mat4 inverse_matrix;

	glm::vec2 viewport_offset;
	glm::vec2 viewport_size;


	glm::vec2 track_position;

public:

	void SetOffset(float x, float y);
	void SetOffsetRelative(float x, float y);

	void SetZoom(float z);
	void SetZoomRelative(float z);


	void CalcMatrixes();

	void SetToScreen(int x, int y, int w, int h);

	void ViewPort(int x, int y, int w, int h);


	SDL_Point WorldToScreen(float x, float y);
	SDL_Rect WorldToScreen(float x, float y, float w, float h);


	glm::vec2 ScreenToWorld(int x, int y);


	void SetTracking(glm::vec2 track);

	void UpdateTracking(float dt);

};


