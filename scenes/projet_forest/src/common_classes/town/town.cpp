#include "town.hpp"

#include <random>
#include <iostream>
#include <cmath>
#define PI 3.14159265359f
using namespace vcl;
using namespace std;

void town::init_town()
{
    init_pate();
    compute_pate(4);
    std::cout << patepos.size() << std::endl;
    for (size_t i = 0; i < patepos.size(); i++)
    {
        //batiments.push_back(mesh_primitive_quadrangle(patepos[i][0], patepos[i][3], patepos[i][2], patepos[i][1]));
        batiments.push_back(create_pate(patepos[i]));
        /*std::cout << patepos[i][0] << std::endl;
        std::cout << patepos[i][1] << std::endl;
        std::cout << patepos[i][2] << std::endl;
        std::cout << patepos[i][3] << std::endl;
        std::cout << " " << std::endl;*/
    }

    d_bat = mesh_drawable(batiments);
}

void town::init_pate()
{
    vector<vec3> initial;
    initial.push_back(vec3(-10.0f, -10.0f, 0));
    initial.push_back(vec3(-10.0f, 10.0f, 0));
    initial.push_back(vec3(10.0f, 10.0f, 0));
    initial.push_back(vec3(10.0f, -10.0f, 0));
    initial.push_back(vec3(0, 0, 0));
    patepos.push_back(initial);
}

vec3 random_divider()
{
    float al = (rand() / (float)RAND_MAX);
    if (al < 0.02f)
    {
        return vec3(1, 0, 0);
    }
    else if (al < 0.07f)
    {
        return vec3(2, 0, 0);
    }
    else
        return vec3(0, 0, 0);
}
/*
Note sur les differents types de subdivision, elles sont codées par un vec3 dans pate qui contient donc 5 elements 

On note pour les types:
Vec3(0,0,0) -> division normale -> on renvoit la division en 4
Vec3(1,0,0) -> création d'un parc -> on arrete à jamais les divisons
Vec3(2,0,0) -> division en cercle pour cette etape

Avoir 1 en coordonnée y signifie qu'on peut pas avoir de cercle à l'interieur
*/
vector<vector<vec3>> town::subdivise(vector<vec3> pate)
{
    vector<vector<vec3>> res;
    if (pate[4].x == 0)
    {
        vector<vec3> current_cube;
        /*
    Avec des data dans cet ordre 
    som0 // som1
    som3 // som2 
    */
        vec3 som0 = pate[0];
        vec3 som1 = pate[1];
        vec3 som2 = pate[2];
        vec3 som3 = pate[3];

        //vec3 translatory = 0.52 * (som1 - som0);
        //vec3 translatorx = 0.52 * (som3 - som0);

        /*
    Pour generer la configuration
    som0 // nvxs1
    nvxs3 // nvxs2
    */
        float decalagey = (0.3f) * rand() / (float)RAND_MAX + 0.35f;
        float decalagex = (0.3f) * rand() / (float)RAND_MAX + 0.35f;
        vec3 nvxs0 = som0;
        vec3 nvxs1 = som0 + (decalagey - 0.02f) * (som1 - som0);
        vec3 nvxs2 = nvxs1 + (decalagex - 0.02f) * (som3 - som0);
        vec3 nvxs3 = som0 + (decalagex - 0.02f) * (som3 - som0);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = som1 + (1 - 0.02f - decalagey) * (som0 - som1);
        nvxs1 = som1;
        nvxs2 = som1 + (decalagex - 0.02f) * (som2 - som1);
        nvxs3 = nvxs2 + (1 - 0.02f - decalagey) * (som3 - som2);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs1 = som2 + (1 - 0.02f - decalagex) * (som1 - som2);
        nvxs0 = nvxs1 + (1 - decalagey - 0.02f) * (som3 - som2);
        nvxs2 = som2;
        nvxs3 = som2 + (1 - decalagey - 0.02f) * (som3 - som2);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = som3 + (1 - decalagex - 0.02f) * (som0 - som3);
        nvxs1 = nvxs0 + (decalagey - 0.02f) * (som1 - som0);
        nvxs2 = som3 + (decalagey - 0.02f) * (som2 - som3);
        nvxs3 = som3;

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();
    }

    if (pate[4].x == 1)
    {
        res.push_back(pate);
    }
    else if (pate[4].x == 2)
    {
        vector<vec3> current_cube;
        float decal = PI / 64;
        float decalh = 0.03f;
        vec3 som0 = pate[0];
        vec3 som1 = pate[1];
        vec3 som2 = pate[2];
        vec3 som3 = pate[3];

        vec3 center = (som1 + som3 + som2 + som0) / 4;
        //float rad = norm(((som1 + som0) / 2 - center));
        vec3 xloc = -((som3 + som0) / 2 - center);
        vec3 yloc = (som1 + som0) / 2 - center;

        vec3 nvxs0 = center + cos(3 * PI / 4 - decal) * xloc + sin(3 * PI / 4 - decal) * yloc;
        vec3 nvxs1 = center + yloc;
        vec3 nvxs2 = center + cos(PI / 4 + decal) * xloc + sin(PI / 4 + decal) * yloc;
        vec3 nvxs3 = center + decalh * yloc;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 1, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(PI / 4 - decal) * xloc + sin(PI / 4 - decal) * yloc;
        nvxs1 = center + xloc;
        nvxs2 = center + cos(-PI / 4 + decal) * xloc + sin(-PI / 4 + decal) * yloc;
        nvxs3 = center + decalh * xloc;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 1, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(-PI / 4 - decal) * xloc + sin(-PI / 4 - decal) * yloc;
        nvxs1 = center - yloc;
        nvxs2 = center + cos(-3 * PI / 4 + decal) * xloc + sin(-3 * PI / 4 + decal) * yloc;
        nvxs3 = center - decalh * yloc;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 1, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(-3 * PI / 4 - decal) * xloc + sin(-3 * PI / 4 - decal) * yloc;
        nvxs1 = center - xloc;
        nvxs2 = center + cos(3 * PI / 4 + decal) * xloc + sin(3 * PI / 4 + decal) * yloc;
        nvxs3 = center - decalh * xloc;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 1, 0));
        res.push_back(current_cube);

        current_cube.clear();
    }

    return res;
}

void town::merge_pat(vector<vector<vec3>> &base, vector<vector<vec3>> to_add)
{
    for (size_t i = 0; i < to_add.size(); i++)
    {
        base.push_back(to_add[i]);
    }
}

void town::compute_pate(int nb)
{
    vector<vector<vec3>> base = patepos;
    for (int j = 0; j < nb; j++)
    {
        vector<vector<vec3>> current;
        for (size_t i = 0; i < base.size(); i++)
        {
            merge_pat(current, subdivise(base[i]));
        }
        base.clear();
        merge_pat(base, current);
        current.clear();
    }
    patepos = base;
}

mesh town::compute_batiment(vector<vec3> coords)
{
    float prob = ((rand() / (float)RAND_MAX));
    // formule pour avoir haut dans[a,b] haut = (b-a)*rand+a
    float haut = (0.7f) * rand() / (float)RAND_MAX + 0.5;
    float scale = (0.15f) * rand() / (float)RAND_MAX + 0.85;
    float rotate = (0.1f) * rand() / (float)RAND_MAX - 0.05;
    float coloral = rand() / (float)RAND_MAX;
    mesh bat;
    vec3 som00 = coords[0];
    vec3 som01 = coords[1];
    vec3 som02 = coords[2];
    vec3 som03 = coords[3];
    vec3 som0 = (scale * som00 + (1 - scale) * som02);
    vec3 som1 = (scale * som01 + (1 - scale) * som03);
    vec3 som2 = (scale * som02 + (1 - scale) * som00);
    vec3 som3 = (scale * som03 + (1 - scale) * som01);
    vec3 up = vec3(0, 0, haut);
    if (prob < 0.7f)
    {
        mesh batim;
        mesh fenetr;
        vec3 nvxs0 = (rotate * som0 + (1 - rotate) * som1);
        vec3 nvxs1 = (rotate * som1 + (1 - rotate) * som2);
        vec3 nvxs2 = (rotate * som2 + (1 - rotate) * som3);
        vec3 nvxs3 = (rotate * som3 + (1 - rotate) * som0);
        batim.push_back(mesh_primitive_cubic_grid(nvxs0, nvxs3, nvxs2, nvxs1, nvxs0 + up, nvxs3 + up, nvxs2 + up, nvxs1 + up, 4, 4, 4));

        fenetr.push_back(compute_windows_on_cube(nvxs0, nvxs3, nvxs2, nvxs1, nvxs0 + up, nvxs3 + up, nvxs2 + up, nvxs1 + up));
        vec3 nvvxs0 = (3 * nvxs0 + nvxs2) / 4;
        vec3 nvvxs1 = (3 * nvxs1 + nvxs3) / 4;
        vec3 nvvxs2 = (nvxs0 + 3 * nvxs2) / 4;
        vec3 nvvxs3 = (nvxs1 + 3 * nvxs3) / 4;
        batim.push_back(mesh_primitive_cubic_grid(nvvxs0 + up, nvvxs3 + up, nvvxs2 + up, nvvxs1 + up, nvvxs0 + up + up, nvvxs3 + up + up, nvvxs2 + up + up, nvvxs1 + up + up, 4, 4, 4));
        fenetr.push_back(compute_windows_on_cube(nvvxs0 + up, nvvxs3 + up, nvvxs2 + up, nvvxs1 + up, nvvxs0 + up + up, nvvxs3 + up + up, nvvxs2 + up + up, nvvxs1 + up + up));
        nvvxs0 = (3 * nvxs0 + 2 * nvxs2) / 5;
        nvvxs1 = (3 * nvxs1 + 2 * nvxs3) / 5;
        nvvxs2 = (2 * nvxs0 + 3 * nvxs2) / 5;
        nvvxs3 = (2 * nvxs1 + 3 * nvxs3) / 5;
        batim.push_back(mesh_primitive_cubic_grid(nvvxs0 + up + up, nvvxs3 + up + up, nvvxs2 + up + up, nvvxs1 + up + up, nvvxs0 + up + up + up, nvvxs3 + up + up + up, nvvxs2 + up + up + up, nvvxs1 + up + up + up, 2, 2, 2));
        fenetr.push_back(compute_windows_on_cube(nvvxs0 + up + up, nvvxs3 + up + up, nvvxs2 + up + up, nvvxs1 + up + up, nvvxs0 + up + up + up, nvvxs3 + up + up + up, nvvxs2 + up + up + up, nvvxs1 + up + up + up));
        if (coloral < 0.5f)
        {
            batim.color.fill(vec3(245, 245, 220) / 255.0f);
        }
        else
        {
            batim.color.fill(vec3(206, 206, 206) / 255.0f); // couleur grise
        }
        bat.push_back(batim);
        bat.push_back(fenetr);
    }
    else if (prob < 1.0f)
    {
        bat.push_back(mesh_primitive_cubic_grid(som0, som3, som2, som1, som0 + 2 * up, som3 + 2 * up, som2 + 2 * up, som1 + 2 * up, 4, 4, 4));
        for (auto i = 0; i < bat.color.size(); i++)
        {
            bat.color[i] = vec3(250, 240, 197) / 255.0f;
        }
        bat.push_back(compute_windows_on_cube(som0, som3, som2, som1, som0 + 2 * up, som3 + 2 * up, som2 + 2 * up, som1 + 2 * up));
    }
    /*else
    {
        vec3 center = (som0 + som1 + som2 + som3) / 4;
        mesh cylindre = mesh_primitive_cylinder(3 * norm((som0 + som1) / 2 - center) / 4, center, center + 2.5f * up, 20, 10);
        mesh cone = mesh_primitive_cone(3 * norm((som0 + som1) / 2 - center) / 4, 0.4 * haut, center + 2.5f * up);
        bat.push_back(cylindre);
        bat.push_back(cone);
    }*/
    return bat;
}

mesh town::compute_garden(vector<vec3> coords)
{
    // Pour compute les arbres, generer un vecteur de positions d'arbres et de hauteur, qui sera ensuite executé au moment du draw
    mesh sol = mesh_primitive_quadrangle(coords[0], coords[3], coords[2], coords[1]);
    sol.color.fill(vec3(0, 1.0f, 0));
    return sol;
}

mesh town::create_pate(vector<vec3> coords)
{
    if (coords[4].x == 0)
    {
        return compute_batiment(coords);
    }
    else if (coords[4].x == 1)
    {
        return compute_garden(coords);
    }
    else
        return compute_batiment(coords);
}

mesh town::compute_windows_on_cube(vec3 const &p000, vec3 const &p100, vec3 const &p110, vec3 const &p010, vec3 const &p001, vec3 const &p101, vec3 const &p111, vec3 const &p011)
{
    mesh res;
    res.push_back(compute_windows_on_quadrangle(p000, p001, p101, p100));
    res.push_back(compute_windows_on_quadrangle(p100, p101, p111, p110));
    res.push_back(compute_windows_on_quadrangle(p110, p111, p101, p010));
    res.push_back(compute_windows_on_quadrangle(p010, p011, p001, p000));
    return res;
}

mesh town::compute_windows_on_quadrangle(vec3 const &p00, vec3 const &p10, vec3 const &p11, vec3 const &p01)
{
    float height = 0.05f;
    float length = 0.1f;
    float decalhoriz = 0.7f * length;
    float decalvert = 0.7f * height;
    vec3 xloc = normalize(p10 - p00);
    vec3 yloc = normalize(p01 - p00);
    //std::cout << norm(p10 - p00) << std::endl;
    //std::cout << norm(p01 - p00) << std::endl;
    //std::cout << " " << std::endl;
    vec3 decalnormal = cross(yloc, xloc);
    vec3 current_hor = p00 + 0.01f * decalnormal;
    vec3 current_vert = p00 + 0.01f * decalnormal;
    //bool ispossiblelong = (norm(p10 - current_hor) > (4 * length));
    //bool ispossiblehaut = (norm(p01 - current_vert) > (4 * height));
    mesh res;
    float imax = floor(norm(p10 - p00) / (length + decalhoriz));
    float jmax = floor(norm(p01 - p00) / (height + decalvert));
    /*std::cout << imax << std::endl;
    std::cout << jmax << std::endl;
    std::cout << " " << std::endl;*/
    for (auto j = 0; j < jmax; j++)
    {
        current_vert = current_vert + decalvert * yloc;
        for (auto i = 0; i < imax; i++)
        {
            current_hor += decalhoriz * xloc;
            vec3 currentp00 = current_hor + current_vert - p00;
            mesh fenetre = mesh_primitive_quadrangle(currentp00, currentp00 + length * xloc, currentp00 + length * xloc + height * yloc, currentp00 + height * yloc);
            float randf = rand() / (float)RAND_MAX;
            if (randf < 0.8f)
            {
                fenetre.color.fill(vec3(240, 255, 255) / 255.0f);
            }
            else
            {
                fenetre.color.fill(vec3(239, 239, 239) / 255.0f);
            }
            res.push_back(fenetre);
            current_hor += length * xloc;
        }
        current_hor = p00;
        current_vert = current_vert + height * yloc;
        //std::cout << current_vert << std::endl;
    }
    return res;
}