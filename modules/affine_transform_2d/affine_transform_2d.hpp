#ifndef AFFINE_TRANSFORM_2D_HPP
#define AFFINE_TRANSFORM_2D_HPP

#include <glm/glm.hpp>


using affine_transform_2d = glm::mat3;

affine_transform_2d translate(const glm::vec2& offset);

affine_transform_2d scale(const float scalar);
affine_transform_2d scale(const glm::vec2& scalars);

affine_transform_2d mirror_x();
affine_transform_2d mirror_y();
affine_transform_2d mirror_origin();

affine_transform_2d rotate(const float angle);
affine_transform_2d rotate(const float c, const float s);

#endif //AFFINE_TRANSFORM_2D_HPP