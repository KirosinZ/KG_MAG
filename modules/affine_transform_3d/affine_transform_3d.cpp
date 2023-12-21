#include "affine_transform_3d.hpp"

namespace engine_3d
{

affine_transform translate(const glm::vec3& offset)
{
	affine_transform id(1.0f);

	id[3][0] = offset.x;
	id[3][1] = offset.y;
	id[3][2] = offset.z;

	return id;
}

affine_transform scale(const float scalar)
{
	affine_transform id(1.0f);

	id[0][0] = scalar;
	id[1][1] = scalar;
	id[2][2] = scalar;

	return id;
}

affine_transform scale(const glm::vec3& scalars)
{
	affine_transform id(1.0f);

	id[0][0] = scalars.x;
	id[1][1] = scalars.y;
	id[2][2] = scalars.z;

	return id;
}

affine_transform mirror_x()
{
	affine_transform id(1.0f);

	id[0][0] = -1;

	return id;
}

affine_transform mirror_y()
{
	affine_transform id(1.0f);

	id[1][1] = -1;

	return id;
}

affine_transform mirror_z()
{
	affine_transform id(1.0f);

	id[2][2] = -1;

	return id;
}

affine_transform mirror_xy()
{
	affine_transform id(1.0f);

	id[0][0] = -1;
	id[1][1] = -1;

	return id;
}

affine_transform mirror_yz()
{
	affine_transform id(1.0f);

	id[1][1] = -1;
	id[2][2] = -1;

	return id;
}

affine_transform mirror_xz()
{
	affine_transform id(1.0f);

	id[2][2] = -1;
	id[0][0] = -1;

	return id;
}

affine_transform mirror_origin()
{
	affine_transform id(1.0f);

	id[0][0] = -1;
	id[1][1] = -1;
	id[2][2] = -1;

	return id;
}

affine_transform rotate_x(const float c, const float s)
{
	const float denom = 1.0f / std::sqrt(c * c + s * s);
	const float cos = c * denom;
	const float sin = s * denom;

	affine_transform id(1.0f);

	id[1][1] = cos; id[2][1] = -sin;
	id[1][2] = sin; id[2][2] = cos;

	return id;
}

affine_transform rotate_y(const float c, const float s)
{
	const float denom = 1.0f / std::sqrt(c * c + s * s);
	const float cos = c * denom;
	const float sin = s * denom;

	affine_transform id(1.0f);

	id[0][0] = cos; id[2][0] = sin;
	id[0][2] = -sin; id[2][2] = cos;

	return id;
}

affine_transform rotate_z(const float c, const float s)
{
	const float denom = 1.0f / std::sqrt(c * c + s * s);
	const float cos = c * denom;
	const float sin = s * denom;

	affine_transform id(1.0f);

	id[0][0] = cos; id[1][0] = -sin;
	id[0][1] = sin; id[1][1] = cos;

	return id;
}

}
