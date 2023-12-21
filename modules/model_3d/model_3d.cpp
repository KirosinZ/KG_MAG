#include "model_3d.hpp"

#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <random>
#include <ranges>
#include <string>
#include <set>

using namespace engine_3d;


model::model(
	std::vector<glm::vec4> vertices,
	std::vector<glm::vec3> colors,
	std::vector<std::pair<int, int>> connectivity,
	std::vector<std::tuple<int, int, int>> triangles)
	: base_vertices_(vertices),
	colors_(std::move(colors)),
	current_vertices_(std::move(vertices)),
	current_origin_(0.0f),
	connectivity_(std::move(connectivity)),
	triangles_(std::move(triangles)),
	accumulated_transform_(1.0f)
{
	base_centroid_ = std::accumulate(base_vertices_.begin(), base_vertices_.end(), glm::vec4(0.0f));
	base_centroid_ /= base_vertices_.size();
	current_centroid_ = base_centroid_;
	current_origin_ = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	std::default_random_engine eng(std::random_device{}());
	std::uniform_int_distribution<int> dist(0, 255);

	for (const auto& _: triangles_)
	{
		triangle_colors_.emplace_back(dist(eng), dist(eng), dist(eng), 0);
	}
}

void model::apply_transform(const affine_transform& transform)
{
	accumulated_transform_ = transform * accumulated_transform_;
	std::ranges::transform(base_vertices_, current_vertices_.begin(), [this](const glm::vec4& vec) {
		return accumulated_transform_ * vec;
		});
	current_origin_ = accumulated_transform_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	current_centroid_ = accumulated_transform_ * base_centroid_;

	local_right_ = accumulated_transform_[0];
	local_up_ = accumulated_transform_[1];
	local_fwd_ = accumulated_transform_[2];
}

model model::from_file(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		std::cerr << "File " << path.filename() << " does not exist!" << std::endl;
		return {};
	}

	if (path.extension().string() != ".obj")
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

	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> colors;
	std::vector<std::pair<int, int>> connectivity;
	std::vector<std::tuple<int, int, int>> triangles;

	struct edge
	{
		edge(int f, int s) : first_index(f), second_index(s)
		{
			if (first_index > second_index)
				std::swap(first_index, second_index);
		}

		int first_index = -1;
		int second_index = -1;

		inline bool operator <(const edge & other) const
		{
			return std::tie(first_index, second_index) < std::tie(other.first_index, other.second_index);
		}
	};

	std::set<edge> edges;

	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);

		if (line.empty())
			continue;

		std::stringstream line_stream(line);
		std::string token;

		line_stream >> token;

		if (token != "v" && token != "f")
			continue;

		if (token == "v")
		{
			glm::vec4 vertex;
			line_stream >> vertex.x >> vertex.y >> vertex.z;
			vertex.w = 1.0f;

			vertices.push_back(vertex);
			colors.emplace_back(0.0f);
		}

		if (token == "f")
		{
			std::vector<int> indices;

			while (!line_stream.eof())
			{
				std::string snippet;
				line_stream >> snippet >> std::ws;

				indices.push_back(std::stoi(snippet) - 1);
			}

			for (int i_ind = 0; i_ind < indices.size(); i_ind++)
			{
				edges.insert(edge(indices[i_ind], indices[(i_ind + 1) % indices.size()]));
			}
			for (int i_ind = 1; i_ind < indices.size() - 1; i_ind++)
			{
				triangles.emplace_back(indices[0], indices[i_ind], indices[i_ind + 1]);
			}
		}
	}
	file.close();

	for (const edge& edge : edges)
	{
		connectivity.emplace_back(edge.first_index, edge.second_index);
	}

	return {
		std::move(vertices),
		std::move(colors),
		std::move(connectivity),
		std::move(triangles),
	};
}