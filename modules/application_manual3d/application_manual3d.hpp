#ifndef APPLICATION_MANUAL3D_HPP
#define APPLICATION_MANUAL3D_HPP

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <vk_boilerplate/boilerplate.hpp>

#include <scene_3d/scene_3d.hpp>
#include <view_3d/view_3d.hpp>
#include <screen_3d/screen_3d.hpp>


namespace engine_3d
{

class application_manual
{
public:
	application_manual(const std::string& window_name, int window_width, int window_height);

	virtual ~application_manual() = default;

	void run();

	void set_scene(const scene& scene);

	virtual void process_input() = 0;
	void draw(const vk::raii::CommandBuffer& cmd);
	virtual void render_to_screen() = 0;
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

	std::pair<vk::raii::ShaderModule, vk::raii::ShaderModule> blank_shaders_{ nullptr, nullptr };
	vk::raii::Pipeline blank_pipeline_ = nullptr;

	void init_rendering();

	screen screen_;

	vk::raii::Buffer screen_buffer_ = nullptr;
	vk::raii::DeviceMemory screen_buffer_memory_ = nullptr;

	vk::raii::Image screen_img_ = nullptr;
	vk::raii::DeviceMemory screen_mem_ = nullptr;
	vk::raii::Sampler screen_sampler_ = nullptr;
	vk::raii::ImageView screen_view_ = nullptr;

	void init_screen();

	vk::raii::CommandPool command_pool_ = nullptr;
	std::vector<vk::raii::CommandBuffer> command_buffers_;
	vk::raii::DescriptorPool descriptor_pool_ = nullptr;
	std::vector<vk::raii::DescriptorSet> descriptor_sets_;
	std::vector<vk::raii::Semaphore> image_acquisition_semaphores_;
	std::vector<vk::raii::Semaphore> present_semaphores_;
	std::vector<vk::raii::Fence> frame_fences_;

	void init_misc();

	scene scene_{};
	view view_{};
};

}

#endif //APPLICATION_MANUAL3D_HPP
