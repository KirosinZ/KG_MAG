#include "camera_2d.hpp"

camera_2d::camera_2d(const float L, const float R, const float B, const float T, const float W, const float H)
	: bottom_left_(L, B),
	upper_right_(R, T),
	aspect_ratio_(W, H)
{}

void camera_2d::move_on(const glm::vec2& offset)
{
	bottom_left_ += offset;
	upper_right_ += offset;
}

void camera_2d::zoom(const glm::vec2& pivot, const float amount)
{
	bottom_left_ = pivot + (bottom_left_ - pivot) * amount;
	upper_right_ = pivot + (upper_right_ - pivot) * amount;
}

void camera_2d::change_aspect_ratio(const glm::vec2& aspect_ratio)
{
	const float old_ratio = aspect_ratio_.x / aspect_ratio_.y;
	const float new_ratio = aspect_ratio.x / aspect_ratio.y;

	const float tpb = upper_right_.y + bottom_left_.y;
	const float tmb = upper_right_.y - bottom_left_.y;

	upper_right_.y = 0.5f * (tpb + tmb * old_ratio / new_ratio);
	bottom_left_.y = 0.5f * (tpb - tmb * old_ratio / new_ratio);

	aspect_ratio_ = aspect_ratio;
}

affine_transform_2d camera_2d::view() const
{
	const glm::vec2 center = 0.5f * (bottom_left_ + upper_right_);
	const glm::vec2 aspect_ratio_compensation = 2.0f / (upper_right_ - bottom_left_);

	return mirror_y() * scale(aspect_ratio_compensation) * translate(-center);
}