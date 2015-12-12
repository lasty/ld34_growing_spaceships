//
// Created by Tristan on 12/12/2015.
//

#include "camera.h"


#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{

}


void Camera::SetOffset(float x, float y)
{
	offset.x = x;
	offset.y = y;

	CalcMatrixes();
}


void Camera::SetOffsetRelative(float x, float y)
{
	offset.x += x;
	offset.y += y;

	SetOffset(offset.x, offset.y);
}


void Camera::SetZoom(float z)
{
	zoom = glm::clamp(z, 0.1f, 4.0f);

	CalcMatrixes();
}


void Camera::SetZoomRelative(float z)
{
	zoom += z;

	SetZoom(zoom);
}



void Camera::ViewPort(int x, int y, int w, int h)
{
	viewport_offset = {x, y};
	viewport_size = {w, h};

	CalcMatrixes();
}


void Camera::SetToScreen(int x, int y, int w, int h)
{
	//ViewPort(x, y, w, h);
	//projection_matrix = glm::ortho(x, x+w, -y+h, y);

	camera_matrix = glm::mat4{};
	projection_matrix = glm::mat4{};

	viewport_size.x = 1.0f;
	viewport_size.y = 1.0f;
	viewport_offset.x = 0.0f;
	viewport_offset.y = 0.0f;
}


void Camera::CalcMatrixes()
{
	float xsize = viewport_size.x * zoom;
	float ysize = viewport_size.y * zoom;

	//projection_matrix = glm::ortho(-viewport_size.x, viewport_size.x, -viewport_size.y, viewport_size.y);
	projection_matrix = glm::ortho(-xsize, xsize, -ysize, ysize);

	camera_matrix = glm::translate( glm::mat4{}, glm::vec3{ offset.x, offset.y, 0.0f } );

	//precalculate inverse matrix for screen to world
	glm::mat4 proj_cam_matrix = projection_matrix * camera_matrix;
	inverse_matrix = glm::inverse(proj_cam_matrix);
}


SDL_Point Camera::WorldToScreen(float x, float y)
{
	//transforms to normalized device coordinates (-1, -1 to +1, +1)
	glm::vec4 p{x, y, 0.0f, 1.0f};
	p = projection_matrix * camera_matrix * p;

	//convert to physical screen coordinates
	p.x = p.x * viewport_size.x + (viewport_size.x / 2.0f);
	p.y = p.y * viewport_size.y + (viewport_size.y / 2.0f);

	return SDL_Point{int(p.x), int(p.y)};
}


SDL_Rect Camera::WorldToScreen(float x, float y, float w, float h)
{
	SDL_Point position = WorldToScreen(x, y);

	SDL_Point position2 = WorldToScreen(x+w, y+h);

	w = position2.x - position.x;
	h = position2.y - position.y;

	return SDL_Rect{position.x, position.y, int(w), int(h)};
}


glm::vec2 Camera::ScreenToWorld(int x, int y)
{
	//convert screen coordinates to normalized device coordinates
	glm::vec4 p {0.0f, 0.0f, 0.0f, 1.0f};
	p.x = (x - (viewport_size.x / 2.0f)) / viewport_size.x;
	p.y = (y - (viewport_size.y / 2.0f)) / viewport_size.y;

	//apply inverse projection matrix
	p = inverse_matrix * p;

	return glm::vec2{p.x, p.y};
}
