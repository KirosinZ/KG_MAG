#ifndef SCENE_3D_HPP
#define SCENE_3D_HPP

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <camera_3d/camera_3d.hpp>
#include <model_3d/model_3d.hpp>


namespace engine_3d
{

class scene
{
public:
	scene() = default;

	inline camera& get_camera()
	{
		return camera_;
	}

	inline std::vector<model>& models()
	{
		return models_;
	}

	inline const std::vector<model>& models() const
	{
		return models_;
	}
private:
	camera camera_{};

	std::vector<model> models_{};
};

}

#endif //SCENE_2D_HPP