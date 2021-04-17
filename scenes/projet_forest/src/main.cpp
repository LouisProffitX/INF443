#include "vcl/vcl.hpp"
#include <iostream>

#include "terrain/terrain.hpp"
#include "tree_Lsystem/tree.hpp"

#include "animation/interpolation.hpp"
#include "scene_helper.hpp"
#include "tree_Lsystem/tree_LSys.hpp"
#include "tree_Lsystem/LSystem.hpp"
#include "terrain/heightmap.hpp"
#include "SkyBox/skybox.hpp"

using namespace vcl;

scene_environment scene;
user_interaction_parameters user;

skybox cube;

buffer<vec3> key_positions;
buffer<float> key_times;
timer_interval timer;
HillAlgorithmParameters params = HillAlgorithmParameters(100, 100, 40, 0, 5.0f, 0.1f, 5.0f);
std::vector<std::vector<float>> gen = generateRandomHeightData(params);

HillAlgorithmParameters params2 = HillAlgorithmParameters();
std::vector<std::vector<float>> genfile = generateFileHeightData("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/textures/heightmap_5.png", params2);
//================================================
//			Variables Declaration
//=================================================

const unsigned int N = 100;
/*const float LOD1 = 5.0f;
const float LOD2 = 10.0f;
const float LOD3 = 20.0f;*/

//================================================
//			Functions declaration
//=================================================

void mouse_move_callback(GLFWwindow *window, double xpos, double ypos);
void window_size_callback(GLFWwindow *window, int width, int height);

void initialize_data();
void display_interface();
void display_scene();
std::vector<float> generate_rotations(int N);
void create_grass(int nbQuad, int ku, int kv);

//================================================
//				Mesh Declaration
//=================================================

mesh terrain;
mesh tree;
mesh_drawable terrain_visual;
mesh_drawable billboard_grass;
perlin_noise_parameters parameters;
mesh_drawable tree_real;

std::vector<float> rot = generate_rotations(N);

int main(int, char *argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	int const width = 1280, height = 1024;
	GLFWwindow *window = create_window(width, height);
	window_size_callback(window, width, height);
	std::cout << opengl_info_display() << std::endl;
	;

	imgui_init(window);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	std::cout << "Initialize data ..." << std::endl;
	initialize_data();

	std::cout << "Start animation loop ..." << std::endl;
	user.fps_record.start();
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		scene.light = scene.camera.position();
		user.fps_record.update();

		glClearColor(0.215f, 0.215f, 0.215f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		imgui_create_frame();
		if (user.fps_record.event)
		{
			std::string const title = "VCL Display - " + str(user.fps_record.fps) + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		ImGui::Begin("GUI", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		user.cursor_on_gui = ImGui::IsAnyWindowFocused();

		if (user.gui.display_frame)
			draw(user.global_frame, scene);

		display_interface();
		display_scene();
		//display_frame();

		ImGui::End();
		imgui_render_frame(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void initialize_data()
{

	//================================================
	//			Initialize default shader
	//=================================================

	GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	GLuint const texture_white = opengl_texture_to_gpu(image_raw{1, 1, image_color_type::rgba, {255, 255, 255, 255}});
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = texture_white;
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;

	//================================================
	//				Initialize camera
	//=================================================

	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	scene.camera.distance_to_center = 4.5f;
	scene.camera.look_at({4, 3, 3}, {0, 0, 3}, {0, 1, 3});

	//================================================
	//				Terrain Declaration
	//=================================================

	terrain = createFromHeightData(genfile, params2);
	std::cout << params2.rows << std::endl;
	terrain_visual = mesh_drawable(terrain);
	//update_terrain(terrain, terrain_visual, parameters);
	/*image_raw const sol = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/textures/texture_heightmap_4.png");
	GLuint const texture_ter_id = opengl_texture_to_gpu(sol, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	terrain_visual.texture = texture_ter_id;*/
	//terrain = mesh_primitive_grid();
	//terrain_visual = mesh_drawable(terrain);

	cube.init_skybox();
	//================================================
	//				Tree Declaration
	//=================================================

	tree = init_tree();

	tree_real = mesh_drawable(tree);

	//================================================
	//			BillBoards Declaration
	//=================================================

	billboard_grass = mesh_drawable(mesh_primitive_quadrangle({-1, 0, 0}, {1, 0, 0}, {1, 0, 2}, {-1, 0, 2}));
	billboard_grass.transform.scale = 0.3f;
	billboard_grass.texture = opengl_texture_to_gpu(image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/textures/grass_texture.png"));
}

void display_scene()
{
	//================================================
	//				Draw terrain
	//=================================================

	draw(terrain_visual, scene);
	//draw_wireframe(terrain_visual, scene);

	draw(cube.dface_devant, scene);
	draw(cube.dface_derriere, scene);
	draw(cube.dface_droite, scene);
	draw(cube.dface_gauche, scene);
	draw(cube.dface_haut, scene);

	//================================================
	//				Draw tree
	//=================================================

	/*draw(tree_real, scene);*/

	//================================================
	//				Draw Billboards
	//=================================================
	/*glDepthMask(false);
	for (unsigned int ku = 0; ku < N; ++ku)
	{
		for (unsigned int kv = 0; kv < N; ++kv)
		{
			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku / (N - 1.0f);
			const float v = kv / (N - 1.0f);
			vec3 p = evaluate_terrain(u, v);
			p.z += parameters.terrain_height * noise_perlin({p.x / 20 + 0.5f, p.y / 20 + 0.5f}, parameters.octave, parameters.persistency, parameters.frequency_gain);
			vec3 distance_with_camera = p - scene.camera.position();
			billboard_grass.transform.translate = p;
			float dist_length = norm(distance_with_camera);

			if (dist_length < LOD1)
			{
				create_grass(3, ku, kv);
			}
			if (dist_length >= LOD1 && dist_length < LOD2)
			{
				create_grass(2, ku, kv);
			}
			if (dist_length >= LOD2 && dist_length < LOD3)
			{
				create_grass(1, ku, kv);
			}
		}
	}
	glDepthMask(true);*/

	//================================================
	//					FIN
	//=================================================
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void display_interface()
{
	ImGui::Checkbox("Frame", &user.gui.display_frame);
	ImGui::Checkbox("Wireframe", &user.gui.display_wireframe);

	bool update = false;
	update |= ImGui::SliderFloat("Persistance", &parameters.persistency, 0.1f, 0.6f);
	update |= ImGui::SliderFloat("Frequency gain", &parameters.frequency_gain, 1.5f, 2.5f);
	update |= ImGui::SliderInt("Octave", &parameters.octave, 1, 8);
	update |= ImGui::SliderFloat("Height", &parameters.terrain_height, 0.1f, 1.5f);

	if (update) // if any slider has been changed - then update the terrain
		update_terrain(terrain, terrain_visual, parameters);

	ImGui::Checkbox("Wireframe", &user.gui.display_wireframe);
}

void window_size_callback(GLFWwindow *, int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	float const fov = 50.0f * pi / 180.0f;
	float const z_min = 0.1f;
	float const z_max = 1000.0f;
	scene.projection = projection_perspective(fov, aspect, z_min, z_max);
	scene.projection_inverse = projection_perspective_inverse(fov, aspect, z_min, z_max);
}

void mouse_move_callback(GLFWwindow *window, double xpos, double ypos)
{
	vec2 const p1 = glfw_get_mouse_cursor(window, xpos, ypos);
	vec2 const &p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	auto &camera = scene.camera;
	if (!user.cursor_on_gui)
	{
		if (state.mouse_click_left && !state.key_ctrl)
			scene.camera.manipulator_rotate_trackball(p0, p1);
		if (state.mouse_click_left && state.key_ctrl)
			camera.manipulator_translate_in_plane(p1 - p0);
		if (state.mouse_click_right)
			camera.manipulator_scale_distance_to_center((p1 - p0).y);
	}
	picking_position(user.picking, key_positions, state, scene, p1);
	user.mouse_prev = p1;
}

std::vector<float> generate_rotations(int N)
{
	std::vector<float> res;
	for (int i = 0; i < N; i++)
	{
		float u = rand_interval() * 3.14f;
		float v = rand_interval() * 3.14f;
		res.push_back(u);
		res.push_back(v);
	}
	return res;
}

void create_grass(int nbQuad, int ku, int kv)
{
	if (nbQuad == 3)
	{
		billboard_grass.transform.scale = 0.1f + rot[ku * kv % (2 * N)] * 0.3f / 3.14f;
		billboard_grass.transform.rotate = rotation();
		draw(billboard_grass, scene);
		billboard_grass.transform.rotate = rotation(vec3{0, 0, 1}, rot[kv]);
		draw(billboard_grass, scene);
		billboard_grass.transform.rotate = rotation(vec3{0, 0, 1}, rot[kv + ku]);
		draw(billboard_grass, scene);
	}
	if (nbQuad == 2)
	{
		billboard_grass.transform.scale = 0.1f + rot[ku * kv % (2 * N)] * 0.3f / 3.14f;
		billboard_grass.transform.rotate = rotation();
		draw(billboard_grass, scene);
		billboard_grass.transform.rotate = rotation(vec3{0, 0, 1}, rot[kv]);
		draw(billboard_grass, scene);
	}
	if (nbQuad == 1)
	{
		billboard_grass.transform.scale = 0.1f + rot[ku * kv % (2 * N)] * 0.3f / 3.14f;
		billboard_grass.transform.rotate = rotation();
		draw(billboard_grass, scene);
	}
}