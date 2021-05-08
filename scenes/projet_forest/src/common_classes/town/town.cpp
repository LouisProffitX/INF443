#include "town.hpp"

#include <random>
#include <iostream>
#include <cmath>
#define PI 3.14159265359f
using namespace vcl;
using namespace std;

void town::init_town(string tow)
{
    init_pate();
    compute_pate(7);
    std::cout << patepos.size() << std::endl;
    for (size_t i = 0; i < patepos.size(); i++)
    {
        batiments.push_back(mesh_primitive_quadrangle(patepos[i][0], patepos[i][3], patepos[i][2], patepos[i][1]));
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
    else if (al < 0.06f)
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
        vec3 nvxs0 = som0;
        vec3 nvxs1 = som0 + 0.48 * (som1 - som0);
        vec3 nvxs2 = nvxs1 + 0.48 * (som3 - som0);
        vec3 nvxs3 = som0 + 0.48 * (som3 - som0);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider());
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = som1 + 0.48 * (som0 - som1);
        nvxs1 = som1;
        nvxs2 = som1 + 0.48 * (som2 - som1);
        nvxs3 = nvxs2 + 0.48 * (som3 - som2);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider());
        res.push_back(current_cube);

        current_cube.clear();

        nvxs1 = som2 + 0.48 * (som1 - som2);
        nvxs0 = nvxs1 + 0.48 * (som3 - som2);
        nvxs2 = som2;
        nvxs3 = som2 + 0.48 * (som3 - som2);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider());
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = som3 + 0.48 * (som0 - som3);
        nvxs1 = nvxs0 + 0.48 * (som1 - som0);
        nvxs2 = som3 + 0.48 * (som2 - som3);
        nvxs3 = som3;

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider());
        res.push_back(current_cube);

        current_cube.clear();
    }
    else if (pate[4].x == 1)
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
        vec3 nvxs3 = center + decalh * nvxs1;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 0, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(PI / 4 - decal) * xloc + sin(PI / 4 - decal) * yloc;
        nvxs1 = center + xloc;
        nvxs2 = center + cos(-PI / 4 + decal) * xloc + sin(-PI / 4 + decal) * yloc;
        nvxs3 = center + decalh * nvxs1;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 0, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(-PI / 4 - decal) * xloc + sin(-PI / 4 - decal) * yloc;
        nvxs1 = center - yloc;
        nvxs2 = center + cos(-3 * PI / 4 + decal) * xloc + sin(-3 * PI / 4 + decal) * yloc;
        nvxs3 = center + decalh * nvxs1;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 0, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(-3 * PI / 4 - decal) * xloc + sin(-3 * PI / 4 - decal) * yloc;
        nvxs1 = center - xloc;
        nvxs2 = center + cos(3 * PI / 4 + decal) * xloc + sin(3 * PI / 4 + decal) * yloc;
        nvxs3 = center + decalh * nvxs1;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 0, 0));
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