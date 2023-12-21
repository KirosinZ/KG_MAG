#ifndef VIEW_2D_HPP
#define VIEW_2D_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <camera_2d/camera_2d.hpp>


class view_2d
{
public:
	view_2d() = default;
	view_2d(GLFWwindow* wnd, camera_2d& cam);

private:
	camera_2d* cam = nullptr;
	glm::vec2 last_cursor_pos;

	static void cursor_pos_callback(GLFWwindow* wnd, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* wnd, double xoffset, double yoffset);
	static void resize_callback(GLFWwindow* wnd, int width, int height);
};

#endif // VIEW_2D_HPP