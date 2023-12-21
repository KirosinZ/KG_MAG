#include "application_3d.hpp"

#include <vk_boilerplate/boilerplate.hpp>

using namespace engine_3d;


application::application(const std::string& window_name, const int window_width, const int window_height)
	: title_(window_name)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	wnd_ = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(glfwCreateWindow(window_width, window_height, window_name.c_str(), nullptr, nullptr), &glfwDestroyWindow);
	glfwSetInputMode(wnd_.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	init();
}

void application::init()
{
	init_env();
	init_display();
	init_rendering();
	init_misc();
}

void application::init_env()
{
	instance_ = boilerplate::instance(title_);
	phys_device_ = std::move(instance_.enumeratePhysicalDevices()[0]);
	queue_family_index_ = boilerplate::get_queue_index(instance_, phys_device_);
	device_ = boilerplate::device(phys_device_, queue_family_index_);
	queue_ = boilerplate::queue(device_, queue_family_index_);
}

void application::init_display()
{
	surface_ = boilerplate::surface(instance_, wnd_.get());
	cache_ = boilerplate::build_swapchain_cache(phys_device_, surface_);
	swapchain_ = boilerplate::swapchain(device_, surface_, cache_, queue_family_index_);
	std::tie(sc_images_, sc_image_views_) = boilerplate::swapchain_images(device_, swapchain_, cache_.image_format);
}

void application::init_rendering()
{
	render_pass_ = boilerplate::render_pass(device_, cache_.image_format);
	framebuffers_ = boilerplate::framebuffers(device_, render_pass_, sc_image_views_, cache_.image_extent);

	std::tie(set_layout_, pipeline_layout_) = boilerplate::layouts(device_);

	line_shaders_ = boilerplate::shaders(device_, "line_shader_3d");
	point_shaders_ = boilerplate::shaders(device_, "line_shader_3d");

	line_pipeline_ = boilerplate::pipeline(device_, line_shaders_.first, line_shaders_.second, pipeline_layout_, render_pass_, vk::PrimitiveTopology::eLineList);
	point_pipeline_ = boilerplate::pipeline(device_, point_shaders_.first, point_shaders_.second, pipeline_layout_, render_pass_, vk::PrimitiveTopology::ePointList);
}

void application::init_misc()
{
	std::tie(command_pool_, command_buffers_) = boilerplate::command_buffers(device_, queue_family_index_, cache_.image_count);
	image_acquisition_semaphores_ = boilerplate::semaphores(device_, cache_.image_count);
	present_semaphores_ = boilerplate::semaphores(device_, cache_.image_count);
	frame_fences_ = boilerplate::fences(device_, cache_.image_count);
}

void application::run()
{
	int current_image_index = 0;
	while (!glfwWindowShouldClose(wnd_.get()))
	{
		glfwPollEvents();

		process_input();

		update_buffers();

		vk::resultCheck(device_.waitForFences(*frame_fences_[current_image_index], true, UINT64_MAX), "fence wait failure");
		auto [result, index] = swapchain_.acquireNextImage(UINT64_MAX, *image_acquisition_semaphores_[current_image_index]);
		vk::resultCheck(result, "Failed to acquire image");
		device_.resetFences(*frame_fences_[current_image_index]);

		command_buffers_[current_image_index].reset();
		vk::CommandBufferBeginInfo bi(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		command_buffers_[current_image_index].begin(bi);

		std::vector<vk::ClearValue> clear_value{
			vk::ClearValue(vk::ClearColorValue(std::array{ 0.6f, 0.6f, 0.6f, 1.0f })),
		};
		vk::RenderPassBeginInfo rpbi(
			*render_pass_,
			*framebuffers_[current_image_index],
			vk::Rect2D({ 0, 0 }, cache_.image_extent),
			clear_value);
		command_buffers_[current_image_index].beginRenderPass(rpbi, vk::SubpassContents::eInline);

		command_buffers_[current_image_index].setViewport(0, vk::Viewport(0.0f, 0.0f, cache_.image_extent.width, cache_.image_extent.height));
		command_buffers_[current_image_index].setScissor(0, vk::Rect2D({ 0, 0 }, cache_.image_extent));

		draw(command_buffers_[current_image_index]);

		command_buffers_[current_image_index].endRenderPass();
		command_buffers_[current_image_index].end();

		const std::vector<vk::PipelineStageFlags> stages{
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
		};
		vk::SubmitInfo si(
			*image_acquisition_semaphores_[current_image_index],
			stages,
			*command_buffers_[current_image_index],
			*present_semaphores_[current_image_index]);
		queue_.submit(si, *frame_fences_[current_image_index]);

		try
		{
			vk::PresentInfoKHR pi(
				*present_semaphores_[current_image_index],
				*swapchain_,
				index);
			queue_.presentKHR(pi);
		}
		catch (...)
		{
			int w = 0;
			int h = 0;
			while (w == 0 || h == 0)
			{
				glfwPollEvents();
				glfwGetWindowSize(wnd_.get(), &w, &h);
			}

			swapchain_ = boilerplate::reassemble_swapchain(device_, phys_device_, surface_, swapchain_, cache_, queue_family_index_);
			std::tie(sc_images_, sc_image_views_) = boilerplate::swapchain_images(device_, swapchain_, cache_.image_format);
			framebuffers_ = boilerplate::framebuffers(device_, render_pass_, sc_image_views_, cache_.image_extent);
		}

		current_image_index = (current_image_index + 1) % cache_.image_count;
	}
	device_.waitIdle();
}

void application::set_scene(const scene& scene)
{
	scene_ = scene;
	view_ = view(wnd_.get(), scene_.get_camera());
	glfwSetWindowUserPointer(wnd_.get(), &view_);

	const int n_models = scene_.models().size();
	vertex_buffers_.reserve(n_models);
	index_buffers_.reserve(n_models);

	for (int m_ind = 0; m_ind < n_models; m_ind++)
	{
		vertex_buffers_.emplace_back(boilerplate::vertex_buffer(device_, phys_device_, scene_.models()[m_ind], queue_family_index_));
		index_buffers_.emplace_back(boilerplate::index_buffer(device_, phys_device_, scene_.models()[m_ind], queue_family_index_));
	}
}

void application::update_buffers()
{
	const int n_models = scene_.models().size();
	const glm::mat4 to_ndc = scene_.get_camera().full_transform();

	for (int m_ind = 0; m_ind < n_models; m_ind++)
	{
		std::vector<glm::vec4> new_vertices = scene_.models()[m_ind].current_vertices();

		const int n_vertices = new_vertices.size();
		for (int v_ind = 0; v_ind < n_vertices; v_ind++)
		{
			new_vertices[v_ind] = to_ndc * new_vertices[v_ind];
		}

		void* data = vertex_buffers_[m_ind].second.mapMemory(0, n_vertices * sizeof(glm::vec4));
		std::memcpy(data, new_vertices.data(), n_vertices * sizeof(glm::vec4));
		vertex_buffers_[m_ind].second.unmapMemory();
	}
}