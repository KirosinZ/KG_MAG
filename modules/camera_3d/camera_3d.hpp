#ifndef CAMERA_3D_HPP
#define CAMERA_3D_HPP

#include <glm/glm.hpp>

#include <affine_transform_3d/affine_transform_3d.hpp>


namespace engine_3d
{

class camera
{
public:
	camera() = default;

	camera(
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
		const float H);

	void move_on(const glm::vec3& offset);
	void rotate(const float yaw, const float pitch);

	void change_aspect_ratio(const glm::vec2& aspect_ratio);

	affine_transform world_to_view() const;
	glm::mat4 view_to_ndc() const;

	glm::mat4 full_transform() const;

	inline const glm::vec2& aspect_ratio() const { return aspect_ratio_; }

	inline const glm::vec3 forward() const { return -screen_normal; }
	inline const glm::vec3 right() const { return glm::normalize(glm::cross(global_up, screen_normal)); }
	inline const glm::vec3 up() const { return glm::cross(right(), forward()); }
private:
	glm::vec3 position;
	glm::vec3 screen_normal;
	glm::vec3 global_up;

	float yaw = 0.0f;
	float pitch = 0.0f;


	float F = 0.0f;
	float D = 0.0f;
	float L = 0.0f;
	float R = 0.0f;
	float B = 0.0f;
	float T = 0.0f;

	glm::vec2 aspect_ratio_{};
};

}

#endif //CAMERA_3D_HPP