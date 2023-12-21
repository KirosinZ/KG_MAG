#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <application_3d/application_3d.hpp>


class application : public engine_3d::application
{
public:

	application(const std::string& window_name, const int window_width, const int window_height);

	void process_input() override;
	void draw(const vk::raii::CommandBuffer& cmd) override;
};

#endif //APPLICATION_HPP