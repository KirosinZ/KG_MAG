#include "view_2d.hpp"

#include <iostream>


view_2d::view_2d(GLFWwindow* wnd, camera_2d& cam)
	: cam(&cam)
{
	if (wnd == nullptr)
		return;

	glfwSetCursorPosCallback(wnd, &cursor_pos_callback);
	glfwSetScrollCallback(wnd, &scroll_callback);
	glfwSetFramebufferSizeCallback(wnd, &resize_callback);
}

void view_2d::cursor_pos_callback(GLFWwindow* wnd, double xpos, double ypos)
{
	void* up = glfwGetWindowUserPointer(wnd);
	if (up == nullptr)
		return;

	view_2d& view = *static_cast<view_2d*>(up);

	if (view.cam == nullptr)
		return;

	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(wnd, &width, &height);

	float x_norm = xpos / width;
	float y_norm = ypos / height;
	y_norm = 1.0f - y_norm;

	glm::vec2 cursor_pos(x_norm, y_norm);
	glm::vec2 screen_diff = cursor_pos - view.last_cursor_pos;

	view.last_cursor_pos = cursor_pos;

	screen_diff *= view.cam->upper_right() - view.cam->bottom_left();

	if (glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		view.cam->move_on(-screen_diff);
}

void view_2d::scroll_callback(GLFWwindow* wnd, double xoffset, double yoffset)
{
	void* up = glfwGetWindowUserPointer(wnd);
	if (up == nullptr)
		return;

	view_2d& view = *static_cast<view_2d*>(up);

	if (view.cam == nullptr)
		return;

	const glm::vec2 pivot = view.cam->bottom_left() + view.last_cursor_pos * (view.cam->upper_right() - view.cam->bottom_left());

	const float extent = std::pow(2.0f, -yoffset);

	view.cam->zoom(pivot, extent);
}

void view_2d::resize_callback(GLFWwindow* wnd, int width, int height)
{
	void* up = glfwGetWindowUserPointer(wnd);
	if (up == nullptr)
		return;

	view_2d& view = *static_cast<view_2d*>(up);

	if (view.cam == nullptr)
		return;

	glm::vec2 new_aspect_ratio(width, height);

	view.cam->change_aspect_ratio(new_aspect_ratio);
}