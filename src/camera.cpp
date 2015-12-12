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
	offset.x -= x;
	offset.y -= y;

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


void Camera::CalcMatrixes()
{
	float xsize = viewport_size.x * zoom;
	float ysize = viewport_size.y * zoom;

	//projection_matrix = glm::ortho(-viewport_size.x, viewport_size.x, -viewport_size.y, viewport_size.y);
	projection_matrix = glm::ortho(-xsize, xsize, -ysize, ysize);

	camera_matrix = glm::translate( glm::mat4{}, glm::vec3{ offset.x, offset.y, 0.0f } );

}


SDL_Point Camera::WorldToScreen(float x, float y)
{
	glm::vec4 p{x, y, 0.0f, 1.0f};

	//p = camera_matrix * projection_matrix * p;
	p = projection_matrix * camera_matrix * p;


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

	//w = p.x;
	//h = p.y;

	return SDL_Rect{position.x, position.y, int(w), int(h)};
}


