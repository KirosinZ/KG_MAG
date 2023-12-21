#ifndef CAMERA_2D_HPP
#define CAMERA_2D_HPP

#include <glm/glm.hpp>

#include <affine_transform_2d/affine_transform_2d.hpp>


class camera_2d
{
public:
	camera_2d() = default;

	camera_2d(const float L, const float R, const float B, const float T, const float W, const float H);

	void move_on(const glm::vec2& offset);
	void zoom(const glm::vec2& pivot, float amount);

	void change_aspect_ratio(const glm::vec2& aspect_ratio);

	affine_transform_2d view() const;

	inline const glm::vec2& bottom_left() const
	{ return bottom_left_; }

	inline const glm::vec2& upper_right() const
	{ return upper_right_; }

	inline const glm::vec2& aspect_ratio() const
	{ return aspect_ratio_; }
private:
	glm::vec2 bottom_left_{};
	glm::vec2 upper_right_{};

	glm::vec2 aspect_ratio_{};
};

#endif //CAMERA_2D_HPP