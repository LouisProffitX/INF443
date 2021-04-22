#include "sphere.hpp"
#include "scene_helper.hpp"

#include <list>
#include <set>

using namespace vcl;

vec3 get_point_on_sphere(vec3 position);
mesh create_isocaedre();
mesh create_sphere();
mesh_drawable get_island_topping(terrain_type _terrain_type);
vec3 get_terrain_color(terrain_type _terrain_type);
mesh get_island_mesh(terrain_type _terrain_type);


planet::planet() {

    /* Configuration du visuel */
    planet_mesh = create_sphere();
    planet_mesh.fill_empty_field();
    planet_visual = mesh_drawable(mesh_primitive_sphere(sphere_radius));
    planet_visual.shading.color = color_sea_low;
    planet_visual.shading.phong = { 0.6 , 0.3, 0, 1 };

    set_islands();
}


void planet::set_islands() 
{

    int number_of_islands = 10;

    islands_centers.resize(number_of_islands);
    terrain_types.resize(number_of_islands);

    vec3 new_center;
    bool changed = true;
    for (int i = 0; i < number_of_islands; i++){
        new_center = get_point_on_sphere(2 * vec3(rand_interval(), rand_interval(), rand_interval()) - vec3(1, 1, 1));
        changed = true;
        while (changed) {
            changed = false;
            new_center = get_point_on_sphere(2 * vec3(rand_interval(), rand_interval(), rand_interval()) - vec3(1, 1, 1));
            for (int j = 0; j < i; j++) {
                if (norm(new_center - islands_centers[j]) < 2 * island_radius) changed = true;
            }
        }
        islands_centers[i] = new_center;
        terrain_types[i] = static_cast<terrain_type>(int(rand_interval() * 5));
    }

    for (int i = 0; i < number_of_islands; i++) {
        for (int j = 0; j < number_of_islands; j++) {
            if (i == j) continue;
            assert_vcl(norm(islands_centers[i] - islands_centers[j]) >= 2 * island_radius, "islands too close");
        }
    }

    for (int i = 0; i < number_of_islands; i++) {
        hierarchy_mesh_drawable island_visual;
        mesh_drawable island = mesh_drawable(get_island_mesh(terrain_types[i]));
        island.shading.color = get_terrain_color(terrain_types[i]);
        island.shading.phong.specular = 0.0f;
        island_visual.add(island, "island");
        island_visual.add(get_island_topping(terrain_types[i]), "topping", "island");
        island_visual["island"].transform.rotate = rotation_between_vector(vec3(0, 0, 1), normalize(islands_centers[i]));
        island_visual["island"].element.shading.color = get_terrain_color(terrain_types[i]);

        island_visual.update_local_to_global_coordinates();

        island_visuals.push_back(island_visual);
    }
}

vec3 get_terrain_color(terrain_type _terrain_type) 
{
    switch (_terrain_type) {
    case terrain_type::CITY : return color_city_low;
    case terrain_type::DESERT: return color_desert_low;
    case terrain_type::FIELD : return color_field_low;
    case terrain_type::FOREST : return color_forest_low;
    case terrain_type::MOUNTAIN : return color_mountain_low;
    default : return vec3(1, 1, 1);
    }
}

mesh_drawable get_island_topping(terrain_type _terrain_type) 
{
    mesh_drawable visual;

    switch (_terrain_type) {
    case terrain_type::CITY :
        visual = mesh_drawable(mesh_load_file_obj("assets/objects/car/car.obj"));
        visual.transform.scale = 0.05;
        visual.transform.translate = vec3(0, 0, sphere_radius);
        visual.transform.rotate = rotation(vec3(1, 0, 0), pi / 2);
        break;
    case terrain_type::DESERT:
        visual = mesh_drawable(mesh_load_file_obj("assets/objects/palm/palm.obj"));
        visual.transform.scale = 0.02;
        visual.transform.translate = vec3(0, 0, sphere_radius);
        visual.transform.rotate = rotation(vec3(1, 0, 0), pi / 2);
        break;
    case terrain_type::FIELD:
        visual = mesh_drawable(mesh_load_file_obj("assets/objects/carrot/carrot.obj"));
        visual.transform.scale = 0.02;
        visual.transform.translate = vec3(0, 0, sphere_radius);
        visual.transform.rotate = rotation(vec3(1, 0, 0), pi / 2);
        break;
    case terrain_type::FOREST:
        visual = mesh_drawable(mesh_load_file_obj("assets/objects/oak/tree.obj"));
        visual.transform.scale = 0.02;
        visual.transform.translate = vec3(0, 0, sphere_radius);
        visual.transform.rotate = rotation(vec3(1, 0, 0), pi / 2);
        break;
    case terrain_type::MOUNTAIN:
        visual = mesh_drawable(mesh_load_file_obj("assets/objects/fir/fir.obj"));
        visual.transform.scale = 0.05;
        visual.transform.translate = vec3(0, 0, sphere_radius);
        visual.transform.rotate = rotation(vec3(1, 0, 0), pi / 2);
        break;
    }

    image_raw texture = image_load_png("assets/textures/lowpoly_palette.png");
    GLuint texture_id = opengl_texture_to_gpu(texture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    visual.texture = texture_id;

    return visual;
}

mesh create_sphere() 
{

    mesh planet_mesh = create_isocaedre();

    unsigned int number_of_triangles, number_of_positions;

    int index_1, index_2, index_3;
    int index_middle_1, index_middle_2, index_middle_3;
    vec3 middle_point_1, middle_point_2, middle_point_3;

    std::map<int, int> middle_points; // Associe le hash_code des deux sommets de l'arr�te avec l'indice du sommet du milieu

    for (int i = 0; i < sphere_division_steps; i++) {

        middle_points.clear();
        number_of_positions = planet_mesh.position.size();
        number_of_triangles = planet_mesh.connectivity.size();

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = planet_mesh.connectivity[j][0];
            index_2 = planet_mesh.connectivity[j][1];
            index_3 = planet_mesh.connectivity[j][2];

            if (middle_points.find(index_1 + number_of_positions * index_2) == middle_points.end()) {
                middle_point_1 = get_point_on_sphere((planet_mesh.position[index_1] + planet_mesh.position[index_2]) / 2);
                planet_mesh.position.push_back(middle_point_1);
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_2, planet_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_1, planet_mesh.position.size() - 1));
            }
            if (middle_points.find(index_2 + number_of_positions * index_3) == middle_points.end()) {
                middle_point_2 = get_point_on_sphere((planet_mesh.position[index_2] + planet_mesh.position[index_3]) / 2);
                planet_mesh.position.push_back(middle_point_2);
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_3, planet_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_2, planet_mesh.position.size() - 1));
            }
            if (middle_points.find(index_3 + number_of_positions * index_1) == middle_points.end()) {
                middle_point_3 = get_point_on_sphere((planet_mesh.position[index_3] + planet_mesh.position[index_1]) / 2);
                planet_mesh.position.push_back(middle_point_3);
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_1, planet_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_3, planet_mesh.position.size() - 1));
            }
        }

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = planet_mesh.connectivity[j][0];
            index_2 = planet_mesh.connectivity[j][1];
            index_3 = planet_mesh.connectivity[j][2];

            // Indice des points milieu du triangle
            index_middle_1 = middle_points.at(index_1 + number_of_positions * index_2);
            index_middle_2 = middle_points.at(index_2 + number_of_positions * index_3);
            index_middle_3 = middle_points.at(index_3 + number_of_positions * index_1);

            planet_mesh.connectivity[j] = uint3(index_1, index_middle_1, index_middle_3);
            planet_mesh.connectivity.push_back(uint3(index_middle_1, index_2, index_middle_2));
            planet_mesh.connectivity.push_back(uint3(index_middle_1, index_middle_2, index_middle_3));
            planet_mesh.connectivity.push_back(uint3(index_middle_2, index_3, index_middle_3));
        }
    }

    return planet_mesh;
}

mesh create_isocaedre()
{
    float phi = (1 + sqrt(5)) / 2.0f; // Nombre d'or
    mesh planet_mesh;

    planet_mesh.position.resize(12);
    planet_mesh.connectivity.resize(20);

    // Ajout des points
    planet_mesh.position[0] = get_point_on_sphere(vec3(-1.0f, phi, 0.0f));
    planet_mesh.position[1] = get_point_on_sphere(vec3(1.0f, phi, 0.0f));
    planet_mesh.position[2] = get_point_on_sphere(vec3(-1.0f, -phi, 0.0f));
    planet_mesh.position[3] = get_point_on_sphere(vec3(1.0f, -phi, 0.0f));
    planet_mesh.position[4] = get_point_on_sphere(vec3(0.0f, -1.0f, phi));
    planet_mesh.position[5] = get_point_on_sphere(vec3(0.0f, 1.0f, phi));
    planet_mesh.position[6] = get_point_on_sphere(vec3(0.0f, -1.0f, -phi));
    planet_mesh.position[7] = get_point_on_sphere(vec3(0.0f, 1.0f, -phi));
    planet_mesh.position[8] = get_point_on_sphere(vec3(phi, 0.0f, -1.0f));
    planet_mesh.position[9] = get_point_on_sphere(vec3(phi, 0.0f, 1.0f));
    planet_mesh.position[10] = get_point_on_sphere(vec3(-phi, 0.0f, -1.0f));
    planet_mesh.position[11] = get_point_on_sphere(vec3(-phi, 0.0f, 1.0f));

    // Triangles de de l'isoca�dre
    planet_mesh.connectivity[0] = uint3(0, 11, 5);
    planet_mesh.connectivity[1] = uint3(0, 5, 1);
    planet_mesh.connectivity[2] = uint3(0, 1, 7);
    planet_mesh.connectivity[3] = uint3(0, 7, 10);
    planet_mesh.connectivity[4] = uint3(0, 10, 11);
    planet_mesh.connectivity[5] = uint3(1, 5, 9);
    planet_mesh.connectivity[6] = uint3(5, 11, 4);
    planet_mesh.connectivity[7] = uint3(11, 10, 2);
    planet_mesh.connectivity[8] = uint3(10, 7, 6);
    planet_mesh.connectivity[9] = uint3(7, 1, 8);
    planet_mesh.connectivity[10] = uint3(3, 9, 4);
    planet_mesh.connectivity[11] = uint3(3, 4, 2);
    planet_mesh.connectivity[12] = uint3(3, 2, 6);
    planet_mesh.connectivity[13] = uint3(3, 6, 8);
    planet_mesh.connectivity[14] = uint3(3, 8, 9);
    planet_mesh.connectivity[15] = uint3(4, 9, 5);
    planet_mesh.connectivity[16] = uint3(2, 4, 11);
    planet_mesh.connectivity[17] = uint3(6, 2, 10);
    planet_mesh.connectivity[18] = uint3(8, 6, 7);
    planet_mesh.connectivity[19] = uint3(9, 8, 1);

    return planet_mesh;
}

vec3 get_point_on_sphere(vec3 position) 
{
    float size = norm(position);
    return position / size * sphere_radius;
}

mesh get_island_mesh(terrain_type _terrain_type)
{
    int n = 4 + rand_interval() * 10;
    float rand_proportion = 2 * island_radius * 3.14f / n * 0.4f;

    mesh surface;

    surface.position.resize(2 * n + 1);
    surface.connectivity.resize(3 * n);

    for (int i = 0; i < n; i++) {
        surface.position[i] = vec3(island_radius * cos(2 * pi * float(i) / n) + rand_proportion * rand_interval(), island_radius * sin(2 * pi * float(i) / n) + rand_proportion  * rand_interval(), sphere_radius);
        surface.position[i + n] = vec3(1.3 * island_radius * cos(2 * pi * float(i) / n), 1.3 * island_radius * sin(2 * pi * float(i) / n), sphere_radius / 2);
    }
    surface.position[2 * n] = vec3(0, 0, sphere_radius);

    for (int i = 0; i < n - 1; i++) {
        surface.connectivity[i] = uint3(i + 1, i, 2 * n);
        surface.connectivity[i + n] = uint3(i, i + 1, n + i + 1);
        surface.connectivity[i + 2 * n] = uint3(i, n + i + 1, n + i);
    }
    surface.connectivity[n - 1] = uint3(0, n - 1, 2 * n);
    surface.connectivity[2 * n - 1] = uint3(n - 1, 0, n);
    surface.connectivity[3 * n - 1] = uint3(n - 1, n, 2 * n - 1);

    surface.fill_empty_field();
    return surface;
}

