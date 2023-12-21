#include <iostream>
#include <fstream>

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "application_manual.h"


engine_3d::model construct_gizmo()
{
	std::vector<glm::vec4> vertices{
		glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f },
		glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f },
		glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f },
		glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f },
		glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f },
		glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f },
	};

	std::vector<glm::vec3> colors{
		glm::vec3{ 1.0f, 0.0f, 0.0f },
		glm::vec3{ 1.0f, 0.0f, 0.0f },
		glm::vec3{ 0.0f, 1.0f, 0.0f },
		glm::vec3{ 0.0f, 1.0f, 0.0f },
		glm::vec3{ 0.0f, 0.0f, 1.0f },
		glm::vec3{ 0.0f, 0.0f, 1.0f },
	};

	std::vector<std::pair<int, int>> connectivity{
		{ 0, 1 },
		{ 2, 3 },
		{ 4, 5 },
	};

	engine_3d::model res(vertices, colors, connectivity);

	return res;
}

int main()
{
	glfwInit();
	const int window_width = 1440;
	const int window_height = 1000;

	::application_manual app("KG_MAG1", window_width, window_height);

	engine_3d::scene scene{};
	engine_3d::model suzanne = engine_3d::model::from_file(std::filesystem::path{ "../models/suzanne.obj" });
	suzanne.apply_transform(engine_3d::scale(0.666f));
	suzanne.apply_transform(engine_3d::translate(glm::vec3{ 2.0f, 0.0f, 0.0f }));
	engine_3d::model melon = engine_3d::model::from_file(std::filesystem::path{ "../models/melon.obj" });
	melon.apply_transform(engine_3d::scale(0.2f));
	melon.apply_transform(engine_3d::rotate_x(0.0f, -1.0f));
	melon.apply_transform(engine_3d::translate(glm::vec3{ 0.0f, -2.0f, 0.0f }));
	engine_3d::model gizmo = construct_gizmo();

	scene.get_camera() = engine_3d::camera(
		glm::vec3(0.0f, 0.0f, 2.0f), 
		glm::vec3(0.0f, 0.0f, 1.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f), 
		0.1f, 
		40.0f, 
		-0.128f, 
		0.128f, 
		-0.096f, 
		0.096f, 
		1440,
		1000);
	scene.models().push_back(suzanne);
	scene.models().push_back(melon);
	// scene.models().push_back(gizmo);

	app.set_scene(scene);

	app.run();

	
	glfwTerminate();
}
