#ifndef MODEL_3D_HPP
#define MODEL_3D_HPP

#include <filesystem>
#include <vector>

#include <glm/glm.hpp>
#include <affine_transform_3d/affine_transform_3d.hpp>


namespace engine_3d
{

class model
{
public:
	model() = default;

	model(
		std::vector<glm::vec4> vertices,
		std::vector<glm::vec3> colors,
		std::vector<std::pair<int, int>> connectivity,
		std::vector<std::tuple<int, int, int>> triangles = {});

	static model from_file(const std::filesystem::path& path);

	void apply_transform(const affine_transform& transform);


public:
	inline const std::vector<glm::vec4>& base_vertices() const
	{
		return base_vertices_;
	}

	inline std::vector<glm::vec3>& colors()
	{
		return colors_;
	}

	inline const std::vector<glm::vec3>& colors() const
	{
		return colors_;
	}

	inline const std::vector<glm::vec4>& current_vertices() const
	{
		return current_vertices_;
	}

	inline const glm::vec4& origin() const
	{
		return current_origin_;
	}

	inline const glm::vec4& centroid() const
	{
		return current_centroid_;
	}

	inline const std::vector<std::pair<int, int>>& connectivity() const
	{
		return connectivity_;
	}

	inline const std::vector<std::tuple<int, int, int>>& triangles() const
	{
		return triangles_;
	}

	inline const std::vector<glm::i8vec4>& triangle_colors() const
	{
		return triangle_colors_;
	}

	inline const affine_transform& transform() const
	{
		return accumulated_transform_;
	}

	inline const glm::vec4& local_right() const { return local_right_; }
	inline glm::vec4& local_right() { return local_right_; }
	inline const glm::vec4& local_up() const { return local_up_; }
	inline glm::vec4& local_up() { return local_up_; }
	inline const glm::vec4& local_fwd() const { return local_fwd_; }
	inline glm::vec4& local_fwd() { return local_fwd_; }
private:
	glm::vec4 local_right_ = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec4 local_up_ = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 local_fwd_ = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

	std::vector<glm::vec4> base_vertices_;
	std::vector<glm::vec3> colors_;
	std::vector<glm::vec4> current_vertices_;

	glm::vec4 base_centroid_;

	glm::vec4 current_origin_;
	glm::vec4 current_centroid_;

	std::vector<std::pair<int, int>> connectivity_;
	std::vector<std::tuple<int, int, int>> triangles_;
	std::vector<glm::i8vec4> triangle_colors_;

	affine_transform accumulated_transform_;
};

}

#endif // MODEL_3D_HPP