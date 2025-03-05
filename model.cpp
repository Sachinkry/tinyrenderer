#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"
#include <cmath>

Model::Model(const char *filename) : verts_(), tex_coords_(), faces_(), tex_indices_()
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++)
                iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (line.substr(0, 3) == "vt ")
        {
            iss >> trash >> trash;
            Vec2f uv;
            iss >> uv.x >> uv.y;
            tex_coords_.push_back(uv);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;  // Vertex indices
            std::vector<int> tf; // Texture indices
            int v, t, n;
            char slash;
            iss >> trash; // 'f'
            while (iss >> v >> slash >> t >> slash >> n)
            {
                v--; // Convert to 0-based
                t--; // Convert to 0-based
                f.push_back(v);
                tf.push_back(t);
            }
            faces_.push_back(f);
            tex_indices_.push_back(tf);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model()
{
}

int Model::nverts()
{
    return (int)verts_.size();
}
Vec2f Model::tex_coord(int i) { return tex_coords_[i]; }

std::vector<int> Model::tex_face(int idx)
{
    return tex_indices_[idx];
}
int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
    return faces_[idx];
}

Vec3f Model::vert(int i)
{
    return verts_[i];
}
