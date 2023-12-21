#ifndef AFFINE_TRANSFORM_3D_HPP
#define AFFINE_TRANSFORM_3D_HPP

#include <glm/glm.hpp>


namespace engine_3d
{

using affine_transform = glm::mat4;

affine_transform translate(const glm::vec3& offset);

affine_transform scale(const float scalar);
affine_transform scale(const glm::vec3& scalars);

affine_transform mirror_x();
affine_transform mirror_y();
affine_transform mirror_z();
affine_transform mirror_xy();
affine_transform mirror_yz();
affine_transform mirror_xz();
affine_transform mirror_origin();

affine_transform rotate_x(const float c, const float s);
affine_transform rotate_y(const float c, const float s);
affine_transform rotate_z(const float c, const float s);

}



#endif //AFFINE_TRANSFORM_3D_HPP