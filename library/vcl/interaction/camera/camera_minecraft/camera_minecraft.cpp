#include "camera_minecraft.hpp"

#include "vcl/base/base.hpp"
#include "../common_functions/common_functions.hpp"

namespace vcl
{

	camera_minecraft::camera_minecraft() : position_camera()
	{
		rotation_z = 0;
		rotation_orthogonal = 0;
		orientation_camera = rotation({1, 0, 0}, pi / 2);
		//orientation_camera_inverse = rotation({1, 0, 0}, pi / 2);
	}

	vec3 camera_minecraft::position() const
	{
		return position_camera;
	}
	rotation camera_minecraft::orientation() const
	{
		return orientation_camera;
	}

	void camera_minecraft::manipulator_rotate_2_axis(float rotation_dorthogonal, float rotation_dz)
	{
		rotation_z += rotation_dz;
		rotation_orthogonal += rotation_dorthogonal;

		orientation_camera = rotation({0, 0, 1}, rotation_z) * rotation({1, 0, 0}, pi / 2 + rotation_orthogonal);
		//orientation_camera_inverse = rotation({0, 0, 1}, rotation_z) * rotation({1, 0, 0}, 2 * rotation_orthogonal - pi);
	}
	void camera_minecraft::manipulator_set_translation(vcl::vec2 const &tr)
	{
		position_camera += translation_in_plane(tr, rotation({0, 0, 1}, rotation_z));
	}

	void camera_minecraft::manipulator_set_altitude(float altitude)
	{
		position_camera.z = altitude;
	}

}