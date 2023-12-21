#include "view_3d.hpp"

#include <iostream>

using namespace engine_3d;


view::view(GLFWwindow* wnd, camera& cam)
	: cam(&cam)
{
	if (wnd == nullptr)
		return;

	glfwSetCursorPosCallback(wnd, &cursor_pos_callback);
	glfwSetFramebufferSizeCallback(wnd, &resize_callback);
}

void view::cursor_pos_callback(GLFWwindow* wnd, double xpos, double ypos)
{
	void* up = glfwGetWindowUserPointer(wnd);
	if (up == nullptr)
		return;

	view& self = *static_cast<view*>(up);

	if (self.cam == nullptr)
		return;

	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(wnd, &width, &height);

	float x_norm = xpos / width;
	float y_norm = ypos / height;

	glm::vec2 cursor_pos(x_norm, y_norm);
	glm::vec2 screen_diff = cursor_pos - self.last_cursor_pos;
	screen_diff *= -45.0f;

	self.last_cursor_pos = cursor_pos;

	if (self.first_ever_input)
	{
		self.first_ever_input = false;
		return;
	}

	self.cam->rotate(screen_diff.x, screen_diff.y);
}

void view::resize_callback(GLFWwindow* wnd, int width, int height)
{
	void* up = glfwGetWindowUserPointer(wnd);
	if (up == nullptr)
		return;

	view& self = *static_cast<view*>(up);

	if (self.cam == nullptr)
		return;

	glm::vec2 new_aspect_ratio(width, height);

	self.cam->change_aspect_ratio(new_aspect_ratio);
}