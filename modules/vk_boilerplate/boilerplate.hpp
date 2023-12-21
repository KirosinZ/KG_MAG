#ifndef VK_BOILERPLATE_HPP
#define VK_BOILERPLATE_HPP

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <model_2d/model_2d.hpp>
#include <model_3d/model_3d.hpp>


namespace boilerplate
{

struct swapchain_cache
{
	int image_count = 0;
	vk::Format image_format;
	vk::ColorSpaceKHR image_color_space;

	vk::Extent2D image_extent;
	vk::SurfaceTransformFlagBitsKHR image_tranform;
};

vk::raii::Instance instance(const std::string& app_name, const uint32_t app_version = VK_MAKE_VERSION(1, 0, 0));

uint32_t get_queue_index(const vk::raii::Instance& instance, const vk::raii::PhysicalDevice& phys_device);

vk::raii::Device device(const vk::raii::PhysicalDevice& phys_device, const uint32_t queue_index);

vk::raii::Queue queue(const vk::raii::Device& device, const uint32_t queue_index);


vk::raii::SurfaceKHR surface(const vk::raii::Instance& instance, GLFWwindow* window);

swapchain_cache build_swapchain_cache(const vk::raii::PhysicalDevice& phys_device, const vk::raii::SurfaceKHR& surface, uint32_t preferred_image_count = 2);

vk::raii::SwapchainKHR swapchain(const vk::raii::Device& device, const vk::raii::SurfaceKHR& surface, const swapchain_cache& cache, const uint32_t queue_family_index);

vk::raii::SwapchainKHR reassemble_swapchain(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const vk::raii::SurfaceKHR& surface, const vk::raii::SwapchainKHR& old_swapchain, swapchain_cache& cache, const uint32_t queue_family_index);

std::pair<std::vector<vk::Image>, std::vector<vk::raii::ImageView>> swapchain_images(const vk::raii::Device& device, const vk::raii::SwapchainKHR& swapchain, const vk::Format image_format);


vk::raii::RenderPass render_pass(const vk::raii::Device& device, const vk::Format image_format);

std::vector<vk::raii::Framebuffer> framebuffers(const vk::raii::Device& device, const vk::raii::RenderPass& render_pass, const std::vector<vk::raii::ImageView>& image_views, const vk::Extent2D image_extent);


std::pair<vk::raii::ShaderModule, vk::raii::ShaderModule> shaders(const vk::raii::Device& device, const std::string& shader_name);

std::pair<vk::raii::DescriptorSetLayout, vk::raii::PipelineLayout> layouts(const vk::raii::Device& device);
std::pair<vk::raii::DescriptorSetLayout, vk::raii::PipelineLayout> layouts_manual(const vk::raii::Device& device);

vk::raii::Pipeline pipeline(const vk::raii::Device& device, const vk::raii::ShaderModule& vertex_shader, const vk::raii::ShaderModule& fragment_shader, const vk::raii::PipelineLayout& layout, const vk::raii::RenderPass& render_pass, const vk::PrimitiveTopology topology);
vk::raii::Pipeline pipeline_manual(const vk::raii::Device& device, const vk::raii::ShaderModule& vertex_shader, const vk::raii::ShaderModule& fragment_shader, const vk::raii::PipelineLayout& layout, const vk::raii::RenderPass& render_pass);

std::pair<vk::raii::CommandPool, std::vector<vk::raii::CommandBuffer>> command_buffers(const vk::raii::Device& device, const int queue_family_index, const int count);

std::vector<vk::raii::Semaphore> semaphores(const vk::raii::Device& device, const int count);

std::vector<vk::raii::Fence> fences(const vk::raii::Device& device, const int count);


std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> vertex_buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const model_2d& model, const uint32_t queue_family_index);

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> index_buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const model_2d& model, const uint32_t queue_family_index);


std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> vertex_buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const engine_3d::model& model, const uint32_t queue_family_index);

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> index_buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const engine_3d::model& model, const uint32_t queue_family_index);

}

#endif //VK_BOILERPLATE_HPP