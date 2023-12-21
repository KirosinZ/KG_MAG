#ifndef APPLICATION_3D_HPP
#define APPLICATION_3D_HPP

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <vk_boilerplate/boilerplate.hpp>

#include <scene_3d/scene_3d.hpp>
#include <view_3d/view_3d.hpp>


namespace engine_3d
{

class application
{
public:

	application(const std::string& window_name, const int window_width, const int window_height);

	void run();

	void set_scene(const scene& scene);

	virtual void process_input() = 0;
	virtual void draw(const vk::raii::CommandBuffer& cmd) = 0;
protected:
	void init();

	std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> wnd_{ nullptr, &glfwDestroyWindow };
	std::string title_{};

	vk::raii::Instance instance_ = nullptr;
	vk::raii::PhysicalDevice phys_device_ = nullptr;
	uint32_t queue_family_index_ = -1;
	vk::raii::Device device_ = nullptr;
	vk::raii::Queue queue_ = nullptr;

	void init_env();

	vk::raii::SurfaceKHR surface_ = nullptr;
	boilerplate::swapchain_cache cache_{};
	vk::raii::SwapchainKHR swapchain_ = nullptr;
	std::vector<vk::Image> sc_images_;
	std::vector<vk::raii::ImageView> sc_image_views_;

	void init_display();

	vk::raii::RenderPass render_pass_ = nullptr;
	std::vector<vk::raii::Framebuffer> framebuffers_;

	vk::raii::DescriptorSetLayout set_layout_ = nullptr;
	vk::raii::PipelineLayout pipeline_layout_ = nullptr;

	std::pair<vk::raii::ShaderModule, vk::raii::ShaderModule> line_shaders_{ nullptr, nullptr };
	std::pair<vk::raii::ShaderModule, vk::raii::ShaderModule> point_shaders_{ nullptr, nullptr };
	vk::raii::Pipeline line_pipeline_ = nullptr;
	vk::raii::Pipeline point_pipeline_ = nullptr;

	void init_rendering();

	vk::raii::CommandPool command_pool_ = nullptr;
	std::vector<vk::raii::CommandBuffer> command_buffers_;
	std::vector<vk::raii::Semaphore> image_acquisition_semaphores_;
	std::vector<vk::raii::Semaphore> present_semaphores_;
	std::vector<vk::raii::Fence> frame_fences_;

	void init_misc();

	scene scene_{};
	view view_{};
	std::vector<std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>> vertex_buffers_;
	std::vector<std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>> index_buffers_;

	void update_buffers();

};

}

#endif //APPLICATION_3D_HPP