#ifndef VIEW_3D_HPP
#define VIEW_3D_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <camera_3d/camera_3d.hpp>


namespace engine_3d
{

class view
{
public:
	view() = default;
	view(GLFWwindow* wnd, camera& cam);

private:
	camera* cam = nullptr;
	glm::vec2 last_cursor_pos{};
	bool first_ever_input = true;

	static void cursor_pos_callback(GLFWwindow* wnd, double xpos, double ypos);
	static void resize_callback(GLFWwindow* wnd, int width, int height);
};

}

#endif // VIEW_3D_HPP