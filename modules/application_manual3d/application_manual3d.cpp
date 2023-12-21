#include "application_manual3d.hpp"

#include <vk_boilerplate/boilerplate.hpp>

using namespace engine_3d;


application_manual::application_manual(const std::string& window_name, const int window_width, const int window_height)
	: title_(window_name)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	wnd_ = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(glfwCreateWindow(window_width, window_height, window_name.c_str(), nullptr, nullptr), &glfwDestroyWindow);
	glfwSetInputMode(wnd_.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	init();
}

void application_manual::init()
{
	init_env();
	init_display();
	init_rendering();
	init_screen();
	init_misc();
}

void application_manual::init_env()
{
	instance_ = boilerplate::instance(title_);
	phys_device_ = std::move(instance_.enumeratePhysicalDevices()[0]);
	queue_family_index_ = boilerplate::get_queue_index(instance_, phys_device_);
	device_ = boilerplate::device(phys_device_, queue_family_index_);
	queue_ = boilerplate::queue(device_, queue_family_index_);
}

void application_manual::init_display()
{
	surface_ = boilerplate::surface(instance_, wnd_.get());
	cache_ = boilerplate::build_swapchain_cache(phys_device_, surface_);
	swapchain_ = boilerplate::swapchain(device_, surface_, cache_, queue_family_index_);
	std::tie(sc_images_, sc_image_views_) = boilerplate::swapchain_images(device_, swapchain_, cache_.image_format);
}

void application_manual::init_rendering()
{
	render_pass_ = boilerplate::render_pass(device_, cache_.image_format);
	framebuffers_ = boilerplate::framebuffers(device_, render_pass_, sc_image_views_, cache_.image_extent);

	std::tie(set_layout_, pipeline_layout_) = boilerplate::layouts_manual(device_);

	blank_shaders_ = boilerplate::shaders(device_, "blank_shader_3d");

	blank_pipeline_ = boilerplate::pipeline_manual(device_, blank_shaders_.first, blank_shaders_.second, pipeline_layout_, render_pass_);
}

void application_manual::init_screen()
{
	screen_ = screen(1000, 1000);

	const vk::Extent2D screen_size{ screen_.size().x, screen_.size().y };

	const vk::DeviceSize size = 4 * screen_size.width * screen_size.height;

	std::vector queue_family_indices{
		queue_family_index_
	};
	vk::BufferCreateInfo bci(
		{},
		size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::SharingMode::eExclusive,
		queue_family_indices);
	screen_buffer_ = device_.createBuffer(bci);

	vk::BufferMemoryRequirementsInfo2 bmri(*screen_buffer_);
	auto reqs = device_.getBufferMemoryRequirements2(bmri);

	vk::PhysicalDeviceMemoryProperties props = phys_device_.getMemoryProperties2().memoryProperties;
	int32_t index = -1;
	for (int mt_ind = 0; mt_ind < props.memoryTypeCount; mt_ind++)
	{
		if (((1 << mt_ind) & reqs.memoryRequirements.memoryTypeBits) != 0)
		{
			if (props.memoryTypes[mt_ind].propertyFlags & (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
				index = mt_ind;
		}
	}

	vk::MemoryAllocateInfo mai(
		size,
		index);
	screen_buffer_memory_ = device_.allocateMemory(mai);

	vk::BindBufferMemoryInfo bbmi(*screen_buffer_, *screen_buffer_memory_, 0);
	device_.bindBufferMemory2(bbmi);

	vk::ImageCreateInfo ici(
		{},
		vk::ImageType::e2D,
		cache_.image_format,
		vk::Extent3D{ screen_size.width, screen_size.height, 1 },
		1,
		1,
		vk::SampleCountFlagBits::e1,
		vk::ImageTiling::eLinear,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::SharingMode::eExclusive,
		queue_family_indices,
		vk::ImageLayout::eUndefined);
	screen_img_ = device_.createImage(ici);

	vk::ImageMemoryRequirementsInfo2 imri2(*screen_img_);
	reqs = device_.getImageMemoryRequirements2(imri2);

	index = -1;
	for (int mt_ind = 0; mt_ind < props.memoryTypeCount; mt_ind++)
	{
		if (((1 << mt_ind) & reqs.memoryRequirements.memoryTypeBits) != 0)
		{
			if (props.memoryTypes[mt_ind].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
				index = mt_ind;
		}
	}

	mai = vk::MemoryAllocateInfo(reqs.memoryRequirements.size, index);
	screen_mem_ = device_.allocateMemory(mai);

	vk::BindImageMemoryInfo bimi(*screen_img_, *screen_mem_, 0);
	device_.bindImageMemory2(bimi);

	vk::ImageViewCreateInfo ivci(
		{},
		*screen_img_,
		vk::ImageViewType::e2D,
		cache_.image_format,
		vk::ComponentMapping{},
		vk::ImageSubresourceRange(
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1));
	screen_view_ = device_.createImageView(ivci);

	vk::SamplerCreateInfo sci(
		{},
		vk::Filter::eNearest,
		vk::Filter::eNearest,
		vk::SamplerMipmapMode::eNearest,
		vk::SamplerAddressMode::eRepeat,
		vk::SamplerAddressMode::eRepeat);
	screen_sampler_ = device_.createSampler(sci);

	vk::DescriptorPoolSize pool_size(vk::DescriptorType::eCombinedImageSampler, 1);
	vk::DescriptorPoolCreateInfo dpci(
		{},
		1,
		pool_size);
	descriptor_pool_ = device_.createDescriptorPool(dpci);

	vk::DescriptorSetAllocateInfo dsai(*descriptor_pool_, *set_layout_);
	descriptor_sets_ = device_.allocateDescriptorSets(dsai);

	vk::DescriptorImageInfo dii(*screen_sampler_, *screen_view_, vk::ImageLayout::eShaderReadOnlyOptimal);
	vk::WriteDescriptorSet wds(*descriptor_sets_[0], 0, 0, vk::DescriptorType::eCombinedImageSampler, dii);
	device_.updateDescriptorSets(wds, {});
}


void application_manual::init_misc()
{
	std::tie(command_pool_, command_buffers_) = boilerplate::command_buffers(device_, queue_family_index_, cache_.image_count);
	image_acquisition_semaphores_ = boilerplate::semaphores(device_, cache_.image_count);
	present_semaphores_ = boilerplate::semaphores(device_, cache_.image_count);
	frame_fences_ = boilerplate::fences(device_, cache_.image_count);
}

void application_manual::run()
{
	int current_image_index = 0;
	while (!glfwWindowShouldClose(wnd_.get()))
	{
		glfwPollEvents();

		process_input();

		render_to_screen();

		vk::resultCheck(device_.waitForFences(*frame_fences_[current_image_index], true, UINT64_MAX), "fence wait failure");
		auto [result, index] = swapchain_.acquireNextImage(UINT64_MAX, *image_acquisition_semaphores_[current_image_index]);
		vk::resultCheck(result, "Failed to acquire image");
		device_.resetFences(*frame_fences_[current_image_index]);

		command_buffers_[current_image_index].reset();
		vk::CommandBufferBeginInfo bi(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		command_buffers_[current_image_index].begin(bi);

		draw(command_buffers_[current_image_index]);

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

		command_buffers_[current_image_index].bindPipeline(vk::PipelineBindPoint::eGraphics, *blank_pipeline_);
		command_buffers_[current_image_index].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layout_, 0, *descriptor_sets_[0], {});
		command_buffers_[current_image_index].draw(3, 1, 0, 0);

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

void application_manual::set_scene(const scene& scene)
{
	scene_ = scene;
	view_ = view(wnd_.get(), scene_.get_camera());
	glfwSetWindowUserPointer(wnd_.get(), &view_);
}

void application_manual::draw(
	const vk::raii::CommandBuffer& cmd)
{
	vk::ImageMemoryBarrier imb(
				vk::AccessFlagBits::eShaderRead,
				vk::AccessFlagBits::eTransferWrite,
				vk::ImageLayout::eShaderReadOnlyOptimal,
				vk::ImageLayout::eTransferDstOptimal,
				VK_QUEUE_FAMILY_IGNORED,
				VK_QUEUE_FAMILY_IGNORED,
				*screen_img_,
				vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
	cmd.pipelineBarrier(vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, imb);

	const vk::DeviceSize screen_size = 4 * screen_.size().x * screen_.size().y;
	void* data = screen_buffer_memory_.mapMemory(0, screen_size);
	std::memcpy(data, screen_.bytes(), screen_size);
	screen_buffer_memory_.unmapMemory();

	vk::BufferImageCopy bic(
		0,
		screen_.size().x,
		screen_.size().y,
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		{},
		vk::Extent3D{ vk::Extent2D{ screen_.size().x, screen_.size().y }, 1 });
	cmd.copyBufferToImage(*screen_buffer_, *screen_img_, vk::ImageLayout::eTransferDstOptimal, bic);

	imb = vk::ImageMemoryBarrier(
			vk::AccessFlagBits::eTransferWrite,
			vk::AccessFlagBits::eShaderRead,
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout::eShaderReadOnlyOptimal,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			*screen_img_,
			vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
	cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, imb);
}

