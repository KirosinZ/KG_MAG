#include "screen_3d.hpp"

#include <algorithm>

using namespace engine_3d;

screen::screen(
	uint32_t width,
	uint32_t height) :
	size_(width, height),
	pixels_(width * height),
	z_buffer_(width * height)
{}

void screen::clear()
{
	pixels_.assign(size_.x * size_.y, {});
	z_buffer_.assign(size_.x * size_.y, 0.0f);
}

glm::i8vec4& screen::pixel(
	uint32_t row,
	uint32_t col)
{
	return pixels_[row * size_.x + col];
}

float& screen::depth(
	uint32_t row,
	uint32_t col)
{
	return z_buffer_[row * size_.x + col];
}

const float& screen::depth(
	uint32_t row,
	uint32_t col) const
{
	return z_buffer_[row * size_.x + col];
}

void screen::draw_triangle(glm::vec3 ndc_v1, glm::vec3 ndc_v2, glm::vec3 ndc_v3, glm::i8vec4 color)
{
	if (glm::normalize(glm::cross(ndc_v2 - ndc_v1, ndc_v3 - ndc_v1)).z >= 0.0f)
		return;

	const auto inside_ndc = [] (glm::vec3 v) -> bool {
		glm::vec3 abs = glm::abs(v);
		return abs.x <= 1.0f
			&& abs.y <= 1.0f
			&& v.z >= 0.0f && v.z <= 1.0f;
	};

	const std::vector visibility{
		inside_ndc(ndc_v1),
		inside_ndc(ndc_v2),
		inside_ndc(ndc_v3),
	};

	if (!std::ranges::any_of(visibility, [](bool flag){ return flag; }))
		return;

	glm::vec2 xy = ndc_v1;
	xy *= size_;
	xy = glm::floor(xy);
	glm::vec3 t0 = glm::vec3(xy, ndc_v1.z);

	xy = ndc_v2;
	xy *= size_;
	xy = glm::floor(xy);
	glm::vec3 t1 = glm::vec3(xy, ndc_v2.z);

	xy = ndc_v3;
	xy *= size_;
	xy = glm::floor(xy);
	glm::vec3 t2 = glm::vec3(xy, ndc_v3.z);

	if (t0.y == t1.y && t0.y == t2.y)
		return;

	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);

	uint32_t total_height = t2.y - t0.y;
	for (int row = 0; row < total_height; row++)
	{
		bool is_second_half = row > t1.y - t0.y || t1.y == t0.y;
		uint32_t segment_height = is_second_half
			? t2.y - t1.y
			: t1.y - t0.y;

		float alpha = float(row) / total_height;
		uint32_t tmp = is_second_half
			? t1.y - t0.y
			: 0;
		float beta = float(row - tmp) / segment_height;


		glm::vec3 A = (t2 - t0) * alpha;
		A.x = glm::round(A.x); A.y = glm::round(A.y);
		A += t0;
		glm::vec3 B = is_second_half
			? (t2 - t1) * beta
			: (t1 - t0) * beta;
		B.x = glm::round(B.x); B.y = glm::round(B.y);
		B += is_second_half
			? t1
			: t0;

		if (A.y < 0.0f || A.y >= float(size_.y)
			|| B.y < 0.0f || B.y >= float(size_.y))
			continue;

		if (A.x > B.x)
			std::swap(A, B);

		for (int col = std::max(A.x, 0.0f); col <= std::min(B.x, float(size_.x - 0.0001f)); col++)
		{
			float phi = B.x == A.x
				? 1.0f
				: float(col - A.x) / (B.x - A.x);

			glm::vec3 P = (B - A) * phi;
			P.x = glm::round(P.x); P.y = glm::round(P.y);
			P += A;

			// if (P.x >= size_.x || P.x < 0.0f
			// 	|| P.y >= size_.y || P.y < 0.0f)
			// 	continue;

			if (depth(P.y, P.x) < P.z) {
				depth(P.y, P.x) = P.z;
				pixel(P.y, P.x) = color;
			}
		}
	}
}
