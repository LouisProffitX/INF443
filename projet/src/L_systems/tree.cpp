#include "tree.hpp"
#include "tree_LSys.hpp"

using namespace vcl;

mesh create_tree_trunk_cylinder(float radius, float height)
{
    mesh m;
    // Number of samples
    const size_t N = 20;

    // Geometry
    for (size_t k = 0; k < N; ++k)
    {
        const float u = k / float(N);
        const vec3 p = {radius * std::cos(2 * 3.14f * u), radius * std::sin(2 * 3.14f * u), 0.0f};
        m.position.push_back(p);
        m.position.push_back(p + vec3(0, 0, height));
    }

    // Connectivity
    for (size_t k = 0; k < N; ++k)
    {
        const unsigned int u00 = 2 * k;
        const unsigned int u01 = (2 * k + 1) % (2 * N);
        const unsigned int u10 = (2 * (k + 1)) % (2 * N);
        const unsigned int u11 = (2 * (k + 1) + 1) % (2 * N);

        const uint3 t1 = {u00, u10, u11};
        const uint3 t2 = {u00, u11, u01};
        m.connectivity.push_back(t1);
        m.connectivity.push_back(t2);
    }

    m.fill_empty_field();
    return m;
}

mesh create_cone(float radius, float height, float z_offset)
{
    mesh m;
    const size_t N = 20;

    for (size_t k = 0; k < N; ++k)
    {
        const float u = k / float(N);
        const vec3 p = {radius * std::cos(2 * 3.14f * u), radius * std::sin(2 * 3.14f * u), 0.0f};
        m.position.push_back(p + vec3{0, 0, z_offset});
    }
    // apex
    m.position.push_back({0, 0, height + z_offset});

    // connectivity
    const unsigned int Ns = N;
    for (unsigned int k = 0; k < Ns; ++k)
    {
        m.connectivity.push_back({k, (k + 1) % Ns, Ns});
    }

    // close the bottom of the cone
    // *************************** //

    // Geometry
    for (size_t k = 0; k < N; ++k)
    {
        const float u = k / float(N);
        const vec3 p = {radius * std::cos(2 * 3.14f * u), radius * std::sin(2 * 3.14f * u), 0.0f};
        m.position.push_back(p + vec3{0, 0, z_offset});
    }
    // central position
    m.position.push_back({0, 0, z_offset});

    // connectivity
    for (unsigned int k = 0; k < Ns; ++k)
        m.connectivity.push_back({k + Ns + 1, (k + 1) % Ns + Ns + 1, 2 * Ns + 1});

    m.fill_empty_field();
    return m;
}

mesh create_tree()
{
    float const h = 0.7f; // trunk height
    float const r = 0.1f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_tree_trunk_cylinder(r, h);
    trunk.color.fill({0.4f, 0.3f, 0.3f});

    // Create a green foliage from 3 cones
    mesh foliage = create_cone(4 * r, 6 * r, 0.0f);      // base-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 2 * r)); // middle-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 4 * r)); // top-cone
    foliage.position += vec3(0, 0, h);                   // place foliage at the top of the trunk
    foliage.color.fill({0.4f, 0.6f, 0.3f});

    // The tree is composted of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}

mesh create_champignons()
{
    float const h = 0.05f; // trunk height
    float const r = 0.01f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_tree_trunk_cylinder(r, h);
    trunk.color.fill({0.5f, 0.5f, 0.5f});

    // Create a red champ from 3 cones
    mesh foliage = create_cone(4 * r, 6 * r, 0.0f); // base-cone
    foliage.position += vec3(0, 0, h);              // place foliage at the top of the trunk
    foliage.color.fill({0.4f, 0.0f, 0.0f});

    // The tree is composted of the trunk and the foliage
    mesh champ = trunk;
    champ.push_back(foliage);

    return champ;
}
