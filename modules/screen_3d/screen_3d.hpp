#ifndef SCREEN_3D_HPP
#define SCREEN_3D_HPP

#include <vector>

#include <glm/glm.hpp>


namespace engine_3d
{

class screen
{
public:
	screen() = default;
	screen(uint32_t width, uint32_t height);

	void clear();
	glm::i8vec4& pixel(uint32_t row, uint32_t col);
	[[nodiscard]] const glm::i8vec4& pixel(uint32_t row, uint32_t col) const;

	float& depth(uint32_t row, uint32_t col);
	[[nodiscard]] const float& depth(uint32_t row, uint32_t col) const;

	[[nodiscard]] glm::uvec2 size() const { return size_; }

	uint8_t* bytes() { return reinterpret_cast<uint8_t*>(pixels_.data()); }

	void draw_triangle(glm::vec3 ndc_v1, glm::vec3 ndc_v2, glm::vec3 ndc_v3, glm::i8vec4 color);
private:
	glm::uvec2 size_{};
	std::vector<glm::i8vec4> pixels_;
	std::vector<float> z_buffer_;
};

}

#endif //SCREEN_3D_HPP
