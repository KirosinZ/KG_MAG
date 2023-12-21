#include "boilerplate.hpp"

#include <iostream>
#include <fstream>

#include <GLFW/glfw3.h>


namespace boilerplate
{

vk::raii::Instance instance(const std::string& app_name, const uint32_t app_version)
{
	vk::ApplicationInfo ai(
		app_name.c_str(),
		app_version,
		"My Engine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_3);

	std::vector<const char*> enabled_instance_layers{
		"VK_LAYER_KHRONOS_validation",
	};

	uint32_t size = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&size);
	std::vector<const char*> enabled_instance_extensions(size);
	std::copy(extensions, std::next(extensions, size), enabled_instance_extensions.begin());

	vk::InstanceCreateInfo ici(
		vk::InstanceCreateFlags{},
		&ai,
		enabled_instance_layers,
		enabled_instance_extensions);
	return vk::raii::Context{}.createInstance(ici);
}

uint32_t get_queue_index(const vk::raii::Instance& instance, const vk::raii::PhysicalDevice& phys_device)
{
	std::vector<vk::QueueFamilyProperties> queue_families = phys_device.getQueueFamilyProperties();

	uint32_t chosen_index = -1;
	for (int f_ind = 0; f_ind < queue_families.size(); f_ind++)
	{
		const vk::QueueFamilyProperties family = queue_families[f_ind];

		const bool graphics_capable = (family.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics;
		const bool present_capable = glfwGetPhysicalDevicePresentationSupport(*instance, *phys_device, f_ind);

		if (graphics_capable && present_capable)
			chosen_index = f_ind;
	}

	return chosen_index;
}

vk::raii::Device device(const vk::raii::PhysicalDevice& phys_device, const uint32_t queue_index)
{
	const std::vector<float> priority{ 1.0f };
	vk::DeviceQueueCreateInfo dqci(
		vk::DeviceQueueCreateFlags{},
		queue_index,
		priority);

	std::vector<const char*> enabled_device_extensions{
		"VK_KHR_swapchain",
	};

	vk::PhysicalDeviceFeatures features = phys_device.getFeatures();

	vk::DeviceCreateInfo dci(
		vk::DeviceCreateFlags{},
		dqci,
		{},
		enabled_device_extensions,
		&features);
	return phys_device.createDevice(dci);
}

vk::raii::Queue queue(const vk::raii::Device& device, const uint32_t queue_index)
{
	return device.getQueue(queue_index, 0);
}


vk::raii::SurfaceKHR surface(const vk::raii::Instance& instance, GLFWwindow* window)
{
	VkSurfaceKHR surface_raw;
	vk::resultCheck(vk::Result(glfwCreateWindowSurface(*instance, window, nullptr, &surface_raw)), "failed to create surface");

	return vk::raii::SurfaceKHR(instance, surface_raw);
}

swapchain_cache build_swapchain_cache(const vk::raii::PhysicalDevice& phys_device, const vk::raii::SurfaceKHR& surface, uint32_t preferred_image_count)
{
	std::vector<vk::SurfaceFormatKHR> surface_formats = phys_device.getSurfaceFormatsKHR(*surface);
	std::vector<vk::PresentModeKHR> present_modes = phys_device.getSurfacePresentModesKHR(*surface);
	vk::SurfaceCapabilitiesKHR surface_capabilities = phys_device.getSurfaceCapabilitiesKHR(*surface);

	swapchain_cache cache;
	cache.image_count = std::clamp(preferred_image_count, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);

	int chosen_format_index = -1;
	for (int f_ind = 0; f_ind < surface_formats.size(); f_ind++)
	{
		vk::SurfaceFormatKHR format = surface_formats[f_ind];

		if (format.format == vk::Format::eB8G8R8A8Srgb)
			chosen_format_index = f_ind;
	}
	if (chosen_format_index == -1)
		throw std::runtime_error("No such format!");

	cache.image_format = surface_formats[chosen_format_index].format;
	cache.image_color_space = surface_formats[chosen_format_index].colorSpace;

	cache.image_extent = surface_capabilities.currentExtent;
	cache.image_tranform = surface_capabilities.currentTransform;

	if (!std::ranges::any_of(present_modes, [](const vk::PresentModeKHR mode) {
		return mode == vk::PresentModeKHR::eFifo;
		}))
		throw std::runtime_error("No FIFO!");

	return cache;
}

vk::raii::SwapchainKHR swapchain(const vk::raii::Device& device, const vk::raii::SurfaceKHR& surface, const swapchain_cache& cache, const uint32_t queue_family_index)
{
	vk::SwapchainCreateInfoKHR sci(
		vk::SwapchainCreateFlagsKHR{},
		*surface,
		cache.image_count,
		cache.image_format,
		cache.image_color_space,
		cache.image_extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		queue_family_index,
		cache.image_tranform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		vk::PresentModeKHR::eFifo);
	return device.createSwapchainKHR(sci);
}

vk::raii::SwapchainKHR reassemble_swapchain(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const vk::raii::SurfaceKHR& surface, const vk::raii::SwapchainKHR& old_swapchain, swapchain_cache& cache, const uint32_t queue_family_index)
{
	vk::SurfaceCapabilitiesKHR surface_capabilities = phys_device.getSurfaceCapabilitiesKHR(*surface);

	cache.image_extent = surface_capabilities.currentExtent;
	cache.image_tranform = surface_capabilities.currentTransform;

	vk::SwapchainCreateInfoKHR sci(
		vk::SwapchainCreateFlagsKHR{},
		*surface,
		cache.image_count,
		cache.image_format,
		cache.image_color_space,
		cache.image_extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		queue_family_index,
		cache.image_tranform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		vk::PresentModeKHR::eFifo,
		false,
		*old_swapchain);
	return device.createSwapchainKHR(sci);
}

std::pair<std::vector<vk::Image>, std::vector<vk::raii::ImageView>> swapchain_images(const vk::raii::Device& device, const vk::raii::SwapchainKHR& swapchain, const vk::Format image_format)
{
	std::vector<VkImage> images_raw = swapchain.getImages();
	std::vector<vk::Image> images(images_raw.size());
	std::ranges::copy(images_raw, images.begin());

	std::vector<vk::raii::ImageView> image_views;
	image_views.reserve(images.size());
	for (int img_ind = 0; img_ind < images.size(); img_ind++)
	{
		vk::ImageViewCreateInfo ivci(
			vk::ImageViewCreateFlags{},
			images[img_ind],
			vk::ImageViewType::e2D,
			image_format,
			vk::ComponentMapping{},
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0,
				1,
				0,
				1));
		image_views.emplace_back(device, ivci);
	}

	return {
		images,
		std::move(image_views),
	};
}

vk::raii::RenderPass render_pass(const vk::raii::Device& device, const vk::Format image_format)
{
	vk::AttachmentDescription ad(
		vk::AttachmentDescriptionFlags{},
		image_format,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference aref(0, vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription sd(
		vk::SubpassDescriptionFlags{},
		vk::PipelineBindPoint::eGraphics,
		{},
		aref);

	vk::SubpassDependency sdep(
		VK_SUBPASS_EXTERNAL,
		0,
		vk::PipelineStageFlagBits::eFragmentShader,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		vk::AccessFlagBits::eShaderRead,
		vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo rpci(
		vk::RenderPassCreateFlags{},
		ad,
		sd,
		sdep);
	return device.createRenderPass(rpci);
}

std::vector<vk::raii::Framebuffer> framebuffers(const vk::raii::Device& device, const vk::raii::RenderPass& render_pass, const std::vector<vk::raii::ImageView>& image_views, const vk::Extent2D image_extent)
{
	std::vector<vk::raii::Framebuffer> framebuffers;
	framebuffers.reserve(image_views.size());
	for (int i_ind = 0; i_ind < image_views.size(); i_ind++)
	{
		vk::FramebufferCreateInfo fci(
			vk::FramebufferCreateFlags{},
			*render_pass,
			*image_views[i_ind],
			image_extent.width,
			image_extent.height,
			1);
		framebuffers.emplace_back(device, fci);
	}

	return framebuffers;
}


std::pair<vk::raii::ShaderModule, vk::raii::ShaderModule> shaders(const vk::raii::Device& device, const std::string& shader_name)
{
	const std::string proper_name_vert = "../shaders/" + shader_name + ".vert.spv";
	const std::string proper_name_frag = "../shaders/" + shader_name + ".frag.spv";

	std::ifstream file(proper_name_vert, std::ios::in | std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("file couldn't be opened");

	int size = file.tellg();
	file.seekg(0);

	std::vector<uint32_t> buffer(size / 4);
	file.read((char*)buffer.data(), size);

	file.close();

	vk::ShaderModuleCreateInfo smci(
		vk::ShaderModuleCreateFlags{},
		buffer);
	vk::raii::ShaderModule vertex_shader = device.createShaderModule(smci);

	file = std::ifstream(proper_name_frag, std::ios::in | std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("file couldn't be opened");

	size = file.tellg();
	file.seekg(0);

	buffer.resize(size / 4);
	file.read((char*)buffer.data(), size);

	file.close();

	smci = vk::ShaderModuleCreateInfo(
		vk::ShaderModuleCreateFlags{},
		buffer);
	vk::raii::ShaderModule fragment_shader = device.createShaderModule(smci);

	return {
		std::move(vertex_shader),
		std::move(fragment_shader)
	};
}

std::pair<vk::raii::DescriptorSetLayout, vk::raii::PipelineLayout> layouts(const vk::raii::Device& device)
{
	vk::DescriptorSetLayoutCreateInfo dslci{};
	vk::raii::DescriptorSetLayout set_layout = device.createDescriptorSetLayout(dslci);

	vk::PipelineLayoutCreateInfo plci{};
	vk::raii::PipelineLayout pipeline_layout = device.createPipelineLayout(plci);

	return {
		std::move(set_layout),
		std::move(pipeline_layout),
	};
}

std::pair<vk::raii::DescriptorSetLayout, vk::raii::PipelineLayout> layouts_manual(const vk::raii::Device& device)
{
	vk::DescriptorSetLayoutBinding dslb(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment);
	vk::DescriptorSetLayoutCreateInfo dslci({}, dslb);
	vk::raii::DescriptorSetLayout set_layout = device.createDescriptorSetLayout(dslci);

	vk::PipelineLayoutCreateInfo plci({}, *set_layout, {});
	vk::raii::PipelineLayout pipeline_layout = device.createPipelineLayout(plci);

	return {
		std::move(set_layout),
		std::move(pipeline_layout),
	};
}

vk::raii::Pipeline pipeline(const vk::raii::Device& device, const vk::raii::ShaderModule& vertex_shader, const vk::raii::ShaderModule& fragment_shader, const vk::raii::PipelineLayout& layout, const vk::raii::RenderPass& render_pass, const vk::PrimitiveTopology topology)
{
	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages{
		vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eVertex,
			*vertex_shader,
			"main"),
		vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eFragment,
			*fragment_shader,
			"main"),
	};

	const std::vector<vk::VertexInputBindingDescription> binding_descriptions{
		vk::VertexInputBindingDescription(0, 4 * sizeof(float), vk::VertexInputRate::eVertex),
		vk::VertexInputBindingDescription(1, 3 * sizeof(float), vk::VertexInputRate::eVertex),
	};
	const std::vector<vk::VertexInputAttributeDescription> attribute_descriptions{
		vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32A32Sfloat, 0),
		vk::VertexInputAttributeDescription(1, 1, vk::Format::eR32G32B32Sfloat, 0),
	};
	vk::PipelineVertexInputStateCreateInfo pvisci(
		vk::PipelineVertexInputStateCreateFlags{},
		binding_descriptions,
		attribute_descriptions);

	vk::PipelineInputAssemblyStateCreateInfo piasci(
		vk::PipelineInputAssemblyStateCreateFlags{},
		topology,
		false);

	vk::PipelineTessellationStateCreateInfo ptsci{};
	vk::PipelineViewportStateCreateInfo pvsci{};

	vk::PipelineRasterizationStateCreateInfo prsci(
		vk::PipelineRasterizationStateCreateFlags{},
		false,
		false,
		topology == vk::PrimitiveTopology::ePointList ? vk::PolygonMode::ePoint : vk::PolygonMode::eLine,
		vk::CullModeFlagBits::eNone);
	prsci.lineWidth = 2.0f;

	vk::PipelineMultisampleStateCreateInfo pmsci(
		vk::PipelineMultisampleStateCreateFlags{},
		vk::SampleCountFlagBits::e1,
		false);

	vk::PipelineDepthStencilStateCreateInfo pdssci(
		vk::PipelineDepthStencilStateCreateFlags{},
		true);
	pdssci.depthCompareOp = vk::CompareOp::eLess;

	vk::PipelineColorBlendAttachmentState color_blend_attachment_state(false);
	color_blend_attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	vk::PipelineColorBlendStateCreateInfo pcbsci(
		vk::PipelineColorBlendStateCreateFlags{},
		false,
		vk::LogicOp::eCopy,
		color_blend_attachment_state,
		{ 0.0f, 0.0f, 0.0f, 0.0f });

	const std::vector<vk::DynamicState> dynamic_states{
		vk::DynamicState::eViewportWithCount,
		vk::DynamicState::eScissorWithCount,
	};
	vk::PipelineDynamicStateCreateInfo pdsci(
		vk::PipelineDynamicStateCreateFlags{},
		dynamic_states);

	vk::GraphicsPipelineCreateInfo gpci(
		vk::PipelineCreateFlags{},
		shader_stages,
		&pvisci,
		&piasci,
		&ptsci,
		&pvsci,
		&prsci,
		&pmsci,
		&pdssci,
		&pcbsci,
		&pdsci,
		*layout,
		*render_pass,
		0);
	return device.createGraphicsPipeline(nullptr, gpci);
}

vk::raii::Pipeline pipeline_manual(const vk::raii::Device& device, const vk::raii::ShaderModule& vertex_shader, const vk::raii::ShaderModule& fragment_shader, const vk::raii::PipelineLayout& layout, const vk::raii::RenderPass& render_pass)
{
	std::vector shader_stages{
		vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eVertex,
			*vertex_shader,
			"main"),
		vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eFragment,
			*fragment_shader,
			"main"),
	};

	vk::PipelineVertexInputStateCreateInfo empty{};

	vk::PipelineInputAssemblyStateCreateInfo piasci(
		vk::PipelineInputAssemblyStateCreateFlags{},
		vk::PrimitiveTopology::eTriangleList);

	vk::PipelineTessellationStateCreateInfo ptsci{};
	vk::PipelineViewportStateCreateInfo pvsci{};

	vk::PipelineRasterizationStateCreateInfo prsci(
		vk::PipelineRasterizationStateCreateFlags{},
		false,
		false,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eNone);

	vk::PipelineMultisampleStateCreateInfo pmsci(
		vk::PipelineMultisampleStateCreateFlags{},
		vk::SampleCountFlagBits::e1,
		false);

	vk::PipelineDepthStencilStateCreateInfo pdssci(
		vk::PipelineDepthStencilStateCreateFlags{},
		false);

	vk::PipelineColorBlendAttachmentState color_blend_attachment_state(false);
	color_blend_attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	vk::PipelineColorBlendStateCreateInfo pcbsci(
		vk::PipelineColorBlendStateCreateFlags{},
		false,
		vk::LogicOp::eCopy,
		color_blend_attachment_state,
		{ 0.0f, 0.0f, 0.0f, 0.0f });

	const std::vector dynamic_states{
		vk::DynamicState::eViewportWithCount,
		vk::DynamicState::eScissorWithCount,
	};
	vk::PipelineDynamicStateCreateInfo pdsci(
		vk::PipelineDynamicStateCreateFlags{},
		dynamic_states);

	vk::GraphicsPipelineCreateInfo gpci(
		vk::PipelineCreateFlags{},
		shader_stages,
		&empty,
		&piasci,
		&ptsci,
		&pvsci,
		&prsci,
		&pmsci,
		&pdssci,
		&pcbsci,
		&pdsci,
		*layout,
		*render_pass,
		0);
	return device.createGraphicsPipeline(nullptr, gpci);
}


std::pair<vk::raii::CommandPool, std::vector<vk::raii::CommandBuffer>> command_buffers(const vk::raii::Device& device, const int queue_family_index, const int count)
{
	vk::CommandPoolCreateInfo cpci(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		queue_family_index);
	vk::raii::CommandPool command_pool = device.createCommandPool(cpci);

	vk::CommandBufferAllocateInfo cbai(
		*command_pool,
		vk::CommandBufferLevel::ePrimary,
		count);
	std::vector<vk::raii::CommandBuffer> command_buffers = device.allocateCommandBuffers(cbai);

	return {
		std::move(command_pool),
		std::move(command_buffers)
	};
}

std::vector<vk::raii::Semaphore> semaphores(const vk::raii::Device& device, const int count)
{
	vk::SemaphoreCreateInfo sci(vk::SemaphoreCreateFlags{});
	std::vector<vk::raii::Semaphore> semaphores;
	semaphores.reserve(count);
	for (int f_ind = 0; f_ind < count; f_ind++)
	{
		semaphores.emplace_back(device, sci);
	}

	return std::move(semaphores);
}

std::vector<vk::raii::Fence> fences(const vk::raii::Device& device, const int count)
{
	vk::FenceCreateInfo fci(vk::FenceCreateFlagBits::eSignaled);
	std::vector<vk::raii::Fence> fences;
	fences.reserve(count);
	for (int f_ind = 0; f_ind < count; f_ind++)
	{
		fences.emplace_back(device, fci);
	}

	return std::move(fences);
}


std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> vertex_buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const model_2d& model, const uint32_t queue_family_index)
{
	const std::vector<uint32_t> queue_family_indices{
		static_cast<uint32_t>(queue_family_index),
	};
	vk::BufferCreateInfo bci(
		vk::BufferCreateFlags{},
		2 * sizeof(glm::vec3) * model.current_vertices().size(),
		vk::BufferUsageFlagBits::eVertexBuffer,
		vk::SharingMode::eExclusive,
		queue_family_indices);
	vk::raii::Buffer vertex_buffer = device.createBuffer(bci);

	vk::BufferMemoryRequirementsInfo2 reqs_info(*vertex_buffer);
	vk::MemoryRequirements reqs = device.getBufferMemoryRequirements2(reqs_info).memoryRequirements;

	vk::PhysicalDeviceMemoryProperties props = phys_device.getMemoryProperties();
	int chosen_type = -1;
	for (int mt_ind = 0; mt_ind < props.memoryTypeCount; mt_ind++)
	{
		if ((reqs.memoryTypeBits & (1 << mt_ind)) == 0)
			continue;

		const vk::MemoryPropertyFlags available_flags = props.memoryTypes[mt_ind].propertyFlags;
		const vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		if ((available_flags & required_flags) == required_flags)
		{
			chosen_type = mt_ind;
			break;
		}
	}

	vk::MemoryAllocateInfo mai(reqs.size, chosen_type);
	vk::raii::DeviceMemory memory = device.allocateMemory(mai);

	vk::BindBufferMemoryInfo bbmi(*vertex_buffer, *memory, 0);
	device.bindBufferMemory2(bbmi);

	void* data = memory.mapMemory(0, sizeof(glm::vec3) * model.current_vertices().size());
	std::memcpy(data, model.current_vertices().data(), sizeof(glm::vec3) * model.current_vertices().size());
	memory.unmapMemory();

	data = memory.mapMemory(sizeof(glm::vec3) * model.current_vertices().size(), sizeof(glm::vec3) * model.current_vertices().size());
	std::memcpy(data, model.colors().data(), sizeof(glm::vec3) * model.current_vertices().size());
	memory.unmapMemory();

	return {
		std::move(vertex_buffer),
		std::move(memory),
	};
}

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> index_buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const model_2d& model, const uint32_t queue_family_index)
{
	const std::vector<uint32_t> queue_family_indices{
		static_cast<uint32_t>(queue_family_index),
	};
	vk::BufferCreateInfo bci(
		vk::BufferCreateFlags{},
		sizeof(std::pair<int, int>) * model.connectivity().size(),
		vk::BufferUsageFlagBits::eIndexBuffer,
		vk::SharingMode::eExclusive,
		queue_family_indices);
	vk::raii::Buffer index_buffer = device.createBuffer(bci);

	vk::BufferMemoryRequirementsInfo2 reqs_info(*index_buffer);
	vk::MemoryRequirements reqs = device.getBufferMemoryRequirements2(reqs_info).memoryRequirements;

	vk::PhysicalDeviceMemoryProperties props = phys_device.getMemoryProperties();
	int chosen_type = -1;
	for (int mt_ind = 0; mt_ind < props.memoryTypeCount; mt_ind++)
	{
		if ((reqs.memoryTypeBits & (1 << mt_ind)) == 0)
			continue;

		const vk::MemoryPropertyFlags available_flags = props.memoryTypes[mt_ind].propertyFlags;
		const vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		if ((available_flags & required_flags) == required_flags)
		{
			chosen_type = mt_ind;
			break;
		}
	}

	vk::MemoryAllocateInfo mai(reqs.size, chosen_type);
	vk::raii::DeviceMemory memory = device.allocateMemory(mai);

	vk::BindBufferMemoryInfo bbmi(*index_buffer, *memory, 0);
	device.bindBufferMemory2(bbmi);

	void* data = memory.mapMemory(0, sizeof(std::pair<int, int>) * model.connectivity().size());
	std::memcpy(data, model.connectivity().data(), sizeof(std::pair<int, int>) * model.connectivity().size());
	memory.unmapMemory();

	return {
		std::move(index_buffer),
		std::move(memory),
	};
}


std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> vertex_buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const engine_3d::model& model, const uint32_t queue_family_index)
{
	const std::vector<uint32_t> queue_family_indices{
	static_cast<uint32_t>(queue_family_index),
	};
	vk::BufferCreateInfo bci(
		vk::BufferCreateFlags{},
		(sizeof(glm::vec3) + sizeof(glm::vec4)) * model.current_vertices().size(),
		vk::BufferUsageFlagBits::eVertexBuffer,
		vk::SharingMode::eExclusive,
		queue_family_indices);
	vk::raii::Buffer vertex_buffer = device.createBuffer(bci);

	vk::BufferMemoryRequirementsInfo2 reqs_info(*vertex_buffer);
	vk::MemoryRequirements reqs = device.getBufferMemoryRequirements2(reqs_info).memoryRequirements;

	vk::PhysicalDeviceMemoryProperties props = phys_device.getMemoryProperties();
	int chosen_type = -1;
	for (int mt_ind = 0; mt_ind < props.memoryTypeCount; mt_ind++)
	{
		if ((reqs.memoryTypeBits & (1 << mt_ind)) == 0)
			continue;

		const vk::MemoryPropertyFlags available_flags = props.memoryTypes[mt_ind].propertyFlags;
		const vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		if ((available_flags & required_flags) == required_flags)
		{
			chosen_type = mt_ind;
			break;
		}
	}

	vk::MemoryAllocateInfo mai(reqs.size, chosen_type);
	vk::raii::DeviceMemory memory = device.allocateMemory(mai);

	vk::BindBufferMemoryInfo bbmi(*vertex_buffer, *memory, 0);
	device.bindBufferMemory2(bbmi);

	void* data = memory.mapMemory(0, sizeof(glm::vec4) * model.current_vertices().size());
	std::memcpy(data, model.current_vertices().data(), sizeof(glm::vec4) * model.current_vertices().size());
	memory.unmapMemory();

	data = memory.mapMemory(sizeof(glm::vec4) * model.current_vertices().size(), sizeof(glm::vec3) * model.current_vertices().size());
	std::memcpy(data, model.colors().data(), sizeof(glm::vec3) * model.current_vertices().size());
	memory.unmapMemory();

	return {
		std::move(vertex_buffer),
		std::move(memory),
	};
}

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> index_buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& phys_device, const engine_3d::model& model, const uint32_t queue_family_index)
{
	const std::vector<uint32_t> queue_family_indices{
		static_cast<uint32_t>(queue_family_index),
	};
	vk::BufferCreateInfo bci(
		vk::BufferCreateFlags{},
		sizeof(std::pair<int, int>) * model.connectivity().size(),
		vk::BufferUsageFlagBits::eIndexBuffer,
		vk::SharingMode::eExclusive,
		queue_family_indices);
	vk::raii::Buffer index_buffer = device.createBuffer(bci);

	vk::BufferMemoryRequirementsInfo2 reqs_info(*index_buffer);
	vk::MemoryRequirements reqs = device.getBufferMemoryRequirements2(reqs_info).memoryRequirements;

	vk::PhysicalDeviceMemoryProperties props = phys_device.getMemoryProperties();
	int chosen_type = -1;
	for (int mt_ind = 0; mt_ind < props.memoryTypeCount; mt_ind++)
	{
		if ((reqs.memoryTypeBits & (1 << mt_ind)) == 0)
			continue;

		const vk::MemoryPropertyFlags available_flags = props.memoryTypes[mt_ind].propertyFlags;
		const vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		if ((available_flags & required_flags) == required_flags)
		{
			chosen_type = mt_ind;
			break;
		}
	}

	vk::MemoryAllocateInfo mai(reqs.size, chosen_type);
	vk::raii::DeviceMemory memory = device.allocateMemory(mai);

	vk::BindBufferMemoryInfo bbmi(*index_buffer, *memory, 0);
	device.bindBufferMemory2(bbmi);

	void* data = memory.mapMemory(0, sizeof(std::pair<int, int>) * model.connectivity().size());
	std::memcpy(data, model.connectivity().data(), sizeof(std::pair<int, int>) * model.connectivity().size());
	memory.unmapMemory();

	return {
		std::move(index_buffer),
		std::move(memory),
	};
}

}

