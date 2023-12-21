#include "affine_transform_2d.hpp"

affine_transform_2d translate(const glm::vec2& offset)
{
	affine_transform_2d id(1.0f);

	id[2][0] = offset.x;
	id[2][1] = offset.y;

	return id;
}

affine_transform_2d scale(const float scalar)
{
	affine_transform_2d id(1.0f);

	id[0][0] = scalar;
	id[1][1] = scalar;

	return id;
}

affine_transform_2d scale(const glm::vec2& scalars)
{
	affine_transform_2d id(1.0f);

	id[0][0] = scalars.x;
	id[1][1] = scalars.y;

	return id;
}

affine_transform_2d mirror_x()
{
	affine_transform_2d id(1.0f);

	id[0][0] = -1;

	return id;
}

affine_transform_2d mirror_y()
{
	affine_transform_2d id(1.0f);

	id[1][1] = -1;

	return id;
}

affine_transform_2d mirror_origin()
{
	affine_transform_2d id(1.0f);

	id[0][0] = -1;
	id[1][1] = -1;

	return id;
}

affine_transform_2d rotate(const float angle)
{
	const float cos = std::cos(angle);
	const float sin = std::sin(angle);

	affine_transform_2d id(1.0f);

	id[0][0] = cos; id[1][0] = -sin;
	id[0][1] = sin; id[1][1] = cos;

	return id;
}

affine_transform_2d rotate(const float c, const float s)
{
	const float denom = 1.0f / (c * c + s * s);
	const float cos = c * denom;
	const float sin = s * denom;

	affine_transform_2d id(1.0f);

	id[0][0] = cos; id[1][0] = -sin;
	id[0][1] = sin; id[1][1] = cos;

	return id;
}
