#include "model_2d.hpp"

#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <ranges>


model_2d::model_2d(
	std::vector<glm::vec3> vertices, 
	std::vector<glm::vec3> colors,
	std::vector<std::pair<int, int>> connectivity)
	: base_vertices_(vertices),
	colors_(std::move(colors)),
	current_vertices_(std::move(vertices)),
	current_origin_(0.0f),
	connectivity_(std::move(connectivity)),
	accumulated_transform_(1.0f)
{
	base_centroid_ = std::accumulate(base_vertices_.begin(), base_vertices_.end(), glm::vec3(0.0f));
	base_centroid_ /= base_vertices_.size();
}

void model_2d::apply_transform(const affine_transform_2d& transform)
{
	accumulated_transform_ = transform * accumulated_transform_;
	std::ranges::transform(base_vertices_, current_vertices_.begin(), [this](const glm::vec3& vec) {
		return accumulated_transform_ * vec;
	});
	current_origin_ = accumulated_transform_ * glm::vec3(0.0f, 0.0f, 1.0f);
	current_centroid_ = accumulated_transform_ * base_centroid_;
}

model_2d model_2d::from_file(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		std::cerr << "File " << path.filename() << " does not exist!" << std::endl;
		return {};
	}

	if (path.extension().string() != ".mdl")
	{
		std::cerr << "File " << path.filename() << " has unsupported extension!" << std::endl;
		return {};
	}

	std::ifstream file(path, std::ios::in);

	if (!file.is_open())
	{
		std::cerr << "File " << path.filename() << " couldn't be opened!" << std::endl;
		return {};
	}

	std::string tmp;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<std::pair<int, int>> connectivity;

	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);

		if (line == "")
			break;

		std::stringstream line_stream(line);

		glm::vec3 vertex;
		line_stream >> vertex.x >> vertex.y >> std::ws;
		vertex.z = 1.0f;

		vertices.push_back(vertex);

		if (line_stream.eof())
			colors.emplace_back(0.0f);
		else
		{
			char semicolon;
			line_stream >> semicolon >> std::ws;
			if (semicolon != ';')
				colors.emplace_back(0.0f);
			else
			{
				glm::vec3 color;
				line_stream >> color.r >> color.g >> color.b >> std::ws;

				colors.push_back(color);
			}
		}
	}

	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);

		std::stringstream line_stream(line);

		std::pair<int, int> edge;
		line_stream >> edge.first >> edge.second >> std::ws;

		connectivity.push_back(edge);
	}

	file.close();

	return {
		std::move(vertices),
		std::move(colors),
		std::move(connectivity),
	};
}