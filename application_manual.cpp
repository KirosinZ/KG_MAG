#include "application_manual.h"

#include <random>
#include <vk_boilerplate/boilerplate.hpp>


application_manual::application_manual(const std::string& window_name, const int window_width, const int window_height)
	: engine_3d::application_manual(window_name,  window_width, window_height)
{
}

void application_manual::process_input()
{
	const auto esc_status = glfwGetKey(wnd_.get(), GLFW_KEY_ESCAPE);
	if (esc_status == GLFW_PRESS)
		glfwSetWindowShouldClose(wnd_.get(), GLFW_TRUE);

	float camera_speed = 0.03f;

	if (glfwGetKey(wnd_.get(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera_speed *= 3.0f;

	engine_3d::model& player_model = scene_.models()[0];
	engine_3d::camera& camera = scene_.get_camera();
	if (glfwGetKey(wnd_.get(), GLFW_KEY_A) == GLFW_PRESS)
		camera.move_on(-camera_speed * camera.right());
	if (glfwGetKey(wnd_.get(), GLFW_KEY_D) == GLFW_PRESS)
		camera.move_on(camera_speed * camera.right());
	if (glfwGetKey(wnd_.get(), GLFW_KEY_W) == GLFW_PRESS)
		camera.move_on(camera_speed * camera.forward());
	if (glfwGetKey(wnd_.get(), GLFW_KEY_S) == GLFW_PRESS)
		camera.move_on(-camera_speed * camera.forward());
	if (glfwGetKey(wnd_.get(), GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.move_on(glm::vec3{ 0.0f, camera_speed, 0.0f });
	if (glfwGetKey(wnd_.get(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.move_on(glm::vec3{ 0.0f, -camera_speed, 0.0f });

	glm::vec3 cur_right = player_model.local_right();
	cur_right = glm::normalize(cur_right);

	if (glfwGetKey(wnd_.get(), GLFW_KEY_I) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(player_model.origin())
			* engine_3d::rotate_z(cur_right.x, cur_right.y)
			* engine_3d::rotate_y(std::sqrt(cur_right.x * cur_right.x + cur_right.y * cur_right.y), -cur_right.z)
			* engine_3d::rotate_x(10.0f, 0.5f)
			* engine_3d::rotate_y(std::sqrt(cur_right.x * cur_right.x + cur_right.y * cur_right.y), cur_right.z)
			* engine_3d::rotate_z(cur_right.x, -cur_right.y)
			* engine_3d::translate(-player_model.origin()));
	if (glfwGetKey(wnd_.get(), GLFW_KEY_K) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(player_model.origin())
			* engine_3d::rotate_z(cur_right.x, cur_right.y)
			* engine_3d::rotate_y(std::sqrt(cur_right.x * cur_right.x + cur_right.y * cur_right.y), -cur_right.z)
			* engine_3d::rotate_x(10.0f, -0.5f)
			* engine_3d::rotate_y(std::sqrt(cur_right.x * cur_right.x + cur_right.y * cur_right.y), cur_right.z)
			* engine_3d::rotate_z(cur_right.x, -cur_right.y)
			* engine_3d::translate(-player_model.origin()));

	glm::vec3 cur_up = player_model.local_up();
	cur_up = glm::normalize(cur_up);

	if (glfwGetKey(wnd_.get(), GLFW_KEY_J) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(player_model.origin())
			* engine_3d::rotate_z(cur_up.y, -cur_up.x)
			* engine_3d::rotate_x(std::sqrt(cur_up.x * cur_up.x + cur_up.y * cur_up.y), cur_up.z)
			* engine_3d::rotate_y(10.0f, 0.5f)
			* engine_3d::rotate_x(std::sqrt(cur_up.x * cur_up.x + cur_up.y * cur_up.y), -cur_up.z)
			* engine_3d::rotate_z(cur_up.y, cur_up.x)
			* engine_3d::translate(-player_model.origin()));
	if (glfwGetKey(wnd_.get(), GLFW_KEY_L) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(player_model.origin())
			* engine_3d::rotate_z(cur_up.y, -cur_up.x)
			* engine_3d::rotate_x(std::sqrt(cur_up.x * cur_up.x + cur_up.y * cur_up.y), cur_up.z)
			* engine_3d::rotate_y(10.0f, -0.5f)
			* engine_3d::rotate_x(std::sqrt(cur_up.x * cur_up.x + cur_up.y * cur_up.y), -cur_up.z)
			* engine_3d::rotate_z(cur_up.y, cur_up.x)
			* engine_3d::translate(-player_model.origin()));

	glm::vec3 cur_fwd = player_model.local_fwd();
	cur_fwd = glm::normalize(cur_fwd);

	if (glfwGetKey(wnd_.get(), GLFW_KEY_U) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(player_model.origin())
			* engine_3d::rotate_y(cur_fwd.z, cur_fwd.x)
			* engine_3d::rotate_x(std::sqrt(cur_fwd.x * cur_fwd.x + cur_fwd.z * cur_fwd.z), -cur_fwd.y)
			* engine_3d::rotate_z(10.0f, 0.5f)
			* engine_3d::rotate_x(std::sqrt(cur_fwd.x * cur_fwd.x + cur_fwd.z * cur_fwd.z), cur_fwd.y)
			* engine_3d::rotate_y(cur_fwd.z, -cur_fwd.x)
			* engine_3d::translate(-player_model.origin()));
	if (glfwGetKey(wnd_.get(), GLFW_KEY_O) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(player_model.origin())
			* engine_3d::rotate_y(cur_fwd.z, cur_fwd.x)
			* engine_3d::rotate_x(std::sqrt(cur_fwd.x * cur_fwd.x + cur_fwd.z * cur_fwd.z), -cur_fwd.y)
			* engine_3d::rotate_z(10.0f, -0.5f)
			* engine_3d::rotate_x(std::sqrt(cur_fwd.x * cur_fwd.x + cur_fwd.z * cur_fwd.z), cur_fwd.y)
			* engine_3d::rotate_y(cur_fwd.z, -cur_fwd.x)
			* engine_3d::translate(-player_model.origin()));

	if (glfwGetKey(wnd_.get(), GLFW_KEY_UP) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(glm::vec3{ 0.0f, 0.0f, -0.05f }));
	if (glfwGetKey(wnd_.get(), GLFW_KEY_DOWN) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(glm::vec3{ 0.0f, 0.0f, 0.05f }));

	if (glfwGetKey(wnd_.get(), GLFW_KEY_LEFT) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(glm::vec3{ -0.05f, 0.0f, 0.0f }));
	if (glfwGetKey(wnd_.get(), GLFW_KEY_RIGHT) == GLFW_PRESS)
		player_model.apply_transform(engine_3d::translate(glm::vec3{ 0.05f, 0.0f, 0.0f }));
}

void application_manual::render_to_screen()
{
	screen_.clear();
	const engine_3d::affine_transform to_ndc = scene_.get_camera().full_transform();

	for (const engine_3d::model& m: scene_.models())
	{
		std::vector<glm::vec4> vertices = m.current_vertices();
		std::vector<glm::vec3> new_vertices(vertices.size());
		std::ranges::transform(vertices, new_vertices.begin(), [&to_ndc, size = screen_.size()] (glm::vec4 v) -> glm::vec3 {
			v = to_ndc * v;
			v /= v.w;
			v.x = 0.5f * (v.x + 1.0f);
			v.y = 0.5f * (v.y + 1.0f);
			v.z = 1.0f - v.z;

			return v;
		});

		for (int t_ind = 0; t_ind < m.triangles().size(); t_ind++)
		{
			const auto&[i1, i2, i3] = m.triangles()[t_ind];
			screen_.draw_triangle(new_vertices[i1], new_vertices[i2], new_vertices[i3], m.triangle_colors()[t_ind]);
		}
	}
}