#include "orbiters.hpp"

using namespace vcl;

hierarchy_mesh_drawable get_plane_mesh_drawable(vec3 rotation_axis, float rotation_angle);
hierarchy_mesh_drawable get_boat_mesh_drawable(vec3 rotation_axis, float rotation_angle);
hierarchy_mesh_drawable get_satelite_mesh_drawable(vec3 rotation_axis, float rotation_angle);
hierarchy_mesh_drawable get_sun_mesh_drawable(vec3 rotation_axis, float rotation_angle);

void orbiter::display()
{
	draw(orbiter_visual, this);
	trajectory_visual.visual.color = vec3(1, 0, 0);
	draw(trajectory_visual, this);
}

void orbiter::update(float time)
{
	rotation_angle = 2 * pi * time * rotation_speed;
	orbiter_visual["root"].transform.rotate = rotation(vec3(0, 1, 0), rotation_angle);
	orbiter_visual.update_local_to_global_coordinates();

	trajectory_visual.add(orbiter_visual["body"].global_transform.translate, time);
}

orbiter::orbiter(orbiter_type _orbiter_type)
{
	rotation_axis = normalize(2 * vec3(rand_interval(), rand_interval(), rand_interval()) - vec3(1, 1, 1));
	rotation_angle = 2 * pi * rand_interval();
	current_rotate = 0;
	switch (_orbiter_type) {
	case orbiter_type::PLANE:
		trajectory_visual = trajectory_drawable(100);
		orbiter_visual = get_plane_mesh_drawable(rotation_axis, rotation_angle);
		rotation_speed = plane_rotation_speed;
		break;
	case orbiter_type::SATELITE:
		trajectory_visual = trajectory_drawable(500);
		orbiter_visual = get_satelite_mesh_drawable(rotation_axis, rotation_angle);
		rotation_speed = satelite_rotation_speed;
		break;
	case orbiter_type::BOAT:
		trajectory_visual = trajectory_drawable(10);
		orbiter_visual = get_boat_mesh_drawable(rotation_axis, rotation_angle);
		rotation_speed = boat_rotation_speed;
		break;
	case orbiter_type::SUN:
		trajectory_visual = trajectory_drawable(1);
		orbiter_visual = get_sun_mesh_drawable(rotation_axis, rotation_angle);
		rotation_speed = sun_rotation_speed;
		break;
	}
}

hierarchy_mesh_drawable get_plane_mesh_drawable(vec3 rotation_axis, float rotation_angle)
{

	hierarchy_mesh_drawable plane_visual;
	GLuint normal_shader = scene_visual::get_shader(shader_type::NORMAL);
	mesh_drawable center = mesh_drawable();
	mesh_drawable rotated_center = mesh_drawable();
	mesh_drawable body = mesh_drawable(mesh_primitive_cylinder(plane_body_radius, { 0, 0, 0 }, { plane_body_length, 0, 0 }), normal_shader);
	mesh_drawable nose = mesh_drawable(mesh_primitive_ellipsoid({ plane_nose_length, plane_body_radius, plane_body_radius }), normal_shader);
	mesh_drawable rear = mesh_drawable(mesh_primitive_ellipsoid({ plane_rear_length, plane_body_radius, plane_body_radius }), normal_shader);
	mesh_drawable left_wing = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { -plane_wing_fleche, -plane_wing_span, 0 }, { -plane_wing_fleche - plane_wing_outer_length, -plane_wing_span, 0 }, { -plane_wing_inner_length, 0, 0 }), normal_shader);
	mesh_drawable right_wing = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { -plane_wing_fleche, plane_wing_span, 0 }, { -plane_wing_fleche - plane_wing_outer_length, plane_wing_span, 0 }, { -plane_wing_inner_length, 0, 0 }), normal_shader);
	mesh_drawable fin = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { plane_fin_inner_length, 0, 0 }, { plane_fin_inner_length - plane_fin_fleche, 0, plane_fin_span }, { plane_fin_inner_length - plane_fin_fleche - plane_fin_outer_length, 0, plane_fin_span }), normal_shader);
	mesh_drawable rear_wing_left = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { plane_rear_wing_inner_length, 0, 0 }, { plane_rear_wing_inner_length - plane_rear_wing_fleche, -plane_rear_wing_span, 0 }, { plane_rear_wing_inner_length - plane_rear_wing_fleche - plane_rear_wing_outer_length, -plane_rear_wing_span, 0 }), normal_shader);
	mesh_drawable rear_wing_right = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { plane_rear_wing_inner_length, 0, 0 }, { plane_rear_wing_inner_length - plane_rear_wing_fleche, plane_rear_wing_span, 0 }, { plane_rear_wing_inner_length - plane_rear_wing_fleche - plane_rear_wing_outer_length, plane_rear_wing_span, 0 }), normal_shader);

	body.shading.color = vec3(0, 0, 1);
	nose.shading.color = vec3(0, 0, 1);
	rear.shading.color = vec3(0, 0, 1);
	left_wing.shading.color = vec3(1, 1, 1);
	right_wing.shading.color = vec3(1, 1, 1);
	fin.shading.color = vec3(1, 1, 1);
	rear_wing_left.shading.color = vec3(1, 1, 1);
	rear_wing_right.shading.color = vec3(1, 1, 1);

	plane_visual.add(center, "center");
	plane_visual.add(rotated_center, "root", "center");
	plane_visual.add(body, "body", "root", vec3(0, 0, plane_altitude));
	plane_visual.add(nose, "nose", "body", { plane_body_length, 0, 0 });
	plane_visual.add(rear, "rear", "body", { 0, 0, 0 });
	plane_visual.add(left_wing, "left_wing", "body", { plane_body_length * plane_wing_advance_body_proportion, -plane_body_radius, 0 });
	plane_visual.add(right_wing, "right_wing", "body", { plane_body_length * plane_wing_advance_body_proportion, plane_body_radius, 0 });
	plane_visual.add(fin, "fin", "body", { 0, 0, plane_body_radius });
	plane_visual.add(rear_wing_left, "rear_wing_left", "body", { 0, -plane_body_radius, 0 });
	plane_visual.add(rear_wing_right, "rear_wing_right", "body", { 0, plane_body_radius, 0 });


	plane_visual["body"].transform.scale = plane_scale;
	plane_visual["center"].transform.rotate = rotation(rotation_axis, rotation_angle);
	plane_visual.update_local_to_global_coordinates();

	return plane_visual;
}

hierarchy_mesh_drawable get_satelite_mesh_drawable(vec3 rotation_axis, float rotation_angle)
{
	hierarchy_mesh_drawable satelite_visual;
	GLuint normal_shader = scene_visual::get_shader(shader_type::NORMAL);

	mesh_drawable center = mesh_drawable();
	mesh_drawable rotated_center = mesh_drawable();
	mesh_drawable body = mesh_drawable(mesh_primitive_cube({ 0, 0, 0 }, satelite_body_height), normal_shader);
	mesh_drawable link = mesh_drawable(mesh_primitive_cylinder(satelite_link_radius, { 0, -satelite_link_length / 2, 0 }, { 0, satelite_link_length / 2, 0 }), normal_shader);
	mesh_drawable panel = mesh_drawable(mesh_primitive_quadrangle(
		{ -satelite_panel_height / 2, -satelite_panel_width / 2, 0 },
		{ satelite_panel_height / 2, -satelite_panel_width / 2, 0 },
		{ satelite_panel_height / 2, satelite_panel_width / 2, 0 },
		{ -satelite_panel_height / 2, satelite_panel_width / 2, 0 }
	), normal_shader);

	body.shading.color = vec3(1, 0.9, 0); // Jaune
	link.shading.color = vec3(0, 0, 0); // Noir
	panel.shading.color = vec3(0, 0, 1);

	satelite_visual.add(center, "center");
	satelite_visual.add(rotated_center, "root", "center");
	satelite_visual.add(body, "body", "root", vec3(0, 0, satelite_altitude));
	satelite_visual.add(link, "left_link", "body", vec3(0, -satelite_body_height / 2 - satelite_link_length / 2, 0));
	satelite_visual.add(link, "right_link", "body", vec3(0, satelite_body_height / 2 + satelite_link_length / 2, 0));
	satelite_visual.add(panel, "left_panel", "left_link", vec3(0, -satelite_link_length / 2 - satelite_panel_width / 2, 0));
	satelite_visual.add(panel, "right_panel", "right_link", vec3(0, satelite_link_length / 2 + satelite_panel_width / 2, 0));

	satelite_visual["left_panel"].transform.rotate = rotation(vec3(0, 1, 0), pi / 4);
	satelite_visual["right_panel"].transform.rotate = rotation(vec3(0, 1, 0), pi / 4);

	satelite_visual["body"].transform.scale = satelite_scale;
	satelite_visual["center"].transform.rotate = rotation(rotation_axis, rotation_angle);
	satelite_visual.update_local_to_global_coordinates();

	return satelite_visual;
}

hierarchy_mesh_drawable get_boat_mesh_drawable(vec3 rotation_axis, float rotation_angle)
{

	hierarchy_mesh_drawable boat_visual;
	GLuint normal_shader = scene_visual::get_shader(shader_type::NORMAL);

	mesh_drawable center = mesh_drawable();
	mesh_drawable rotated_center = mesh_drawable();
	mesh_drawable body = mesh_drawable(mesh_load_file_obj("assets/objects/boat/boat.obj"), normal_shader);

	body.shading.color = color_sea_high;

	boat_visual.add(center, "center");
	boat_visual.add(rotated_center, "root", "center");
	boat_visual.add(body, "body", "root", vec3(0, 0, boat_altitude));

	boat_visual["body"].transform.scale = boat_scale;
	boat_visual["center"].transform.rotate = rotation(rotation_axis, rotation_angle);
	boat_visual.update_local_to_global_coordinates();

	return boat_visual;
}

hierarchy_mesh_drawable get_sun_mesh_drawable(vec3 rotation_axis, float rotation_angle)
{

	hierarchy_mesh_drawable sun_visual;
	GLuint normal_shader = scene_visual::get_shader(shader_type::SUN);

	mesh_drawable center = mesh_drawable();
	mesh_drawable rotated_center = mesh_drawable();
	mesh_drawable body = mesh_drawable(mesh_primitive_sphere(sun_radius), normal_shader);

	body.shading.color = vec3(1.0, 1.0, 0.0);

	sun_visual.add(center, "center");
	sun_visual.add(rotated_center, "root", "center");
	sun_visual.add(body, "body", "root", vec3(0, 0, sun_altitude));

	sun_visual["center"].transform.rotate = rotation(rotation_axis, rotation_angle);
	sun_visual.update_local_to_global_coordinates();

	return sun_visual;
}

