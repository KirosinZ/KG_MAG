#include "camera_3d.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace engine_3d;

camera::camera(
	const glm::vec3& eye,
	const glm::vec3& screen_normal,
	const glm::vec3& global_up,
	const float F,
	const float D,
	const float L,
	const float R,
	const float B,
	const float T,
	const float W,
	const float H) :
	position(eye),
	screen_normal(glm::normalize(screen_normal)),
	global_up(global_up),
	F(F),
	D(D),
	L(L),
	R(R),
	B(B),
	T(T),
	aspect_ratio_(W, H)
{

}

void camera::move_on(const glm::vec3& offset)
{
	position += offset;
}

void camera::rotate(const float yaw, const float pitch)
{
	this->pitch = glm::clamp(this->pitch + pitch, -89.0f, 89.0f);
	this->yaw += yaw;

	const float cos_yaw = glm::cos(glm::radians(this->yaw));
	const float sin_yaw = glm::sin(glm::radians(this->yaw));
	const float cos_pitch = glm::cos(glm::radians(this->pitch));
	const float sin_pitch = glm::sin(glm::radians(this->pitch));

	screen_normal = glm::vec3(sin_yaw * cos_pitch, -sin_pitch, cos_yaw * cos_pitch);
}

void camera::change_aspect_ratio(const glm::vec2& aspect_ratio)
{
	const float old_ratio = aspect_ratio_.x / aspect_ratio_.y;
	const float new_ratio = aspect_ratio.x / aspect_ratio.y;

	const float tpb = T + B;
	const float tmb = T - B;

	T = 0.5f * (tpb + tmb * old_ratio / new_ratio);
	B = 0.5f * (tpb - tmb * old_ratio / new_ratio);

	aspect_ratio_ = aspect_ratio;
}

affine_transform camera::world_to_view() const
{
	const glm::vec3 o_v = position - F * screen_normal;
	const glm::vec3 k_v = glm::normalize(screen_normal);
	const glm::vec3 i_v = glm::normalize(glm::cross(global_up, k_v));
	const glm::vec3 j_v = glm::cross(k_v, i_v);

	affine_transform res(1.0f);
	res[0] = glm::vec4(i_v, 0.0f);
	res[1] = glm::vec4(j_v, 0.0f);
	res[2] = glm::vec4(k_v, 0.0f);
	res[3] = glm::vec4(o_v, 1.0f);

	return glm::inverse(res);
}

glm::mat4 camera::view_to_ndc() const
{
	const float i_rl = 1.0f / (R - L);
	const float i_tb = 1.0f / (T - B);
	const float i_f = 1.0f / F;
	glm::mat4 res(1.0f);
	res[0][0] = 2.0f * i_rl;
	res[1][1] = -2.0f * i_tb;
	res[2] = i_f * glm::vec4((L + R) * i_rl, (B + T) * i_tb, -(2.0f * F + D) / D, -1.0f);
	res[3] = glm::vec4(-(L + R) * i_rl, -(T + B) * i_tb, -1.0f, 1.0f);

	return res;
}

glm::mat4 camera::full_transform() const
{
	return view_to_ndc() * world_to_view();
}