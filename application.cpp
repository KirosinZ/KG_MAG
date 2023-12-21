#include "application.hpp"

#include <vk_boilerplate/boilerplate.hpp>


application::application(const std::string& window_name, const int window_width, const int window_height)
	: engine_3d::application(window_name,  window_width, window_height)
{
}

void application::process_input()
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

void application::draw(const vk::raii::CommandBuffer& cmd)
{
	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *line_pipeline_);
	for (int m_ind = 0; m_ind < scene_.models().size(); m_ind++)
	{
		cmd.bindVertexBuffers(0, { *vertex_buffers_[m_ind].first, *vertex_buffers_[m_ind].first }, { 0, sizeof(glm::vec4) * scene_.models()[m_ind].base_vertices().size() });
		cmd.bindIndexBuffer(*index_buffers_[m_ind].first, 0, vk::IndexType::eUint32);

		cmd.drawIndexed(2 * scene_.models()[m_ind].connectivity().size(), 1, 0, 0, 0);
	}
	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *point_pipeline_);
	for (int m_ind = 0; m_ind < scene_.models().size(); m_ind++)
	{
		cmd.bindVertexBuffers(0, { *vertex_buffers_[m_ind].first, *vertex_buffers_[m_ind].first }, { 0, sizeof(glm::vec4) * scene_.models()[m_ind].base_vertices().size() });

		cmd.draw(scene_.models()[m_ind].base_vertices().size(), 1, 0, 0);
	}
}