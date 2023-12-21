#ifndef APPLICATION_MANUAL_H
#define APPLICATION_MANUAL_H

#include <application_manual3d/application_manual3d.hpp>


class application_manual : public engine_3d::application_manual
{
public:

	application_manual(const std::string& window_name, const int window_width, const int window_height);

	void process_input() override;
	void render_to_screen() override;
};

#endif //APPLICATION_MANUAL_H
