#ifndef MODEL_2D_HPP
#define MODEL_2D_HPP

#include <filesystem>
#include <vector>

#include <glm/glm.hpp>
#include <affine_transform_2d/affine_transform_2d.hpp>


class model_2d
{
public:
	model_2d() = default;

	model_2d(std::vector<glm::vec3> vertices, std::vector<glm::vec3> colors, std::vector<std::pair<int, int>> connectivity);

	static model_2d from_file(const std::filesystem::path& path);

	void apply_transform(const affine_transform_2d& transform);


public:
	inline const std::vector<glm::vec3>& base_vertices() const
	{ return base_vertices_; }

	inline const std::vector<glm::vec3>& colors() const
	{ return colors_; }

	inline const std::vector<glm::vec3>& current_vertices() const
	{ return current_vertices_; }

	inline const glm::vec3& origin() const
	{ return current_origin_; }

	inline const glm::vec3& centroid() const
	{ return current_centroid_; }

	inline const std::vector<std::pair<int, int>>& connectivity() const
	{ return connectivity_; }

	inline const affine_transform_2d& transform() const
	{ return accumulated_transform_; }
private:
	std::vector<glm::vec3> base_vertices_;
	std::vector<glm::vec3> colors_;
	std::vector<glm::vec3> current_vertices_;

	glm::vec3 base_centroid_;

	glm::vec3 current_origin_;
	glm::vec3 current_centroid_;

	std::vector<std::pair<int, int>> connectivity_;

	affine_transform_2d accumulated_transform_;
};

#endif // MODEL_2D_HPP