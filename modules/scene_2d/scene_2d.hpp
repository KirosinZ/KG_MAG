#ifndef SCENE_2D_HPP
#define SCENE_2D_HPP

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <camera_2d/camera_2d.hpp>
#include <model_2d/model_2d.hpp>


class scene_2d
{
public:
	scene_2d() = default;

	inline camera_2d& camera()
	{ return camera_; }

	inline std::vector<model_2d>& models()
	{ return models_; }

	inline const std::vector<model_2d>& models() const
	{ return models_; }
private:
	camera_2d camera_{};

	std::vector<model_2d> models_{};
};

#endif //SCENE_2D_HPP