// main.cpp
#include <iostream>
#include <map>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "shaders.h" // Our new shading module

// Global config
static const int width = 800;
static const int height = 800;

// Some example colors
static const TGAColor white(255, 255, 255, 255);
static const TGAColor materialColor(139, 69, 19, 255);

// Light direction
static const Vec3f light_dir(0, 0, -1);

// Convert from [-1..1] in model space to screen space
Vec3f world2screen(const Vec3f &v)
{
    return Vec3f(int((v.x + 1.f) * width / 2.f + 0.5f),
                 int((v.y + 1.f) * height / 2.f + 0.5f),
                 v.z);
}

int main(int argc, char **argv)
{
    // Load model
    Model *model = nullptr;
    if (argc == 2)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("assets/models/diablo3_pose.obj");
    }

    // Load texture
    TGAImage texture;
    texture.read_tga_file("assets/models/diablo3_pose_nm.tga");
    texture.flip_vertically();

    // Prepare z-buffer
    float *zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
    {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    // Create output image
    TGAImage image(width, height, TGAImage::RGB);

    // Compute vertex normals by averaging adjacent face normals
    std::vector<Vec3f> vertexNormals(model->nverts(), Vec3f(0, 0, 0));
    std::map<int, std::vector<int>> vertexToFaces;

    // Map each vertex to faces that contain it
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            vertexToFaces[face[j]].push_back(i);
        }
    }

    // Accumulate face normals per vertex
    for (int vertIdx = 0; vertIdx < model->nverts(); vertIdx++)
    {
        const std::vector<int> &faces = vertexToFaces[vertIdx];
        Vec3f sumNormal(0, 0, 0);
        for (int faceIdx : faces)
        {
            std::vector<int> face = model->face(faceIdx);
            Vec3f v0 = model->vert(face[0]);
            Vec3f v1 = model->vert(face[1]);
            Vec3f v2 = model->vert(face[2]);
            Vec3f normal = (v2 - v0) ^ (v1 - v0);
            sumNormal = sumNormal + normal.normalize();
        }
        vertexNormals[vertIdx] = sumNormal.normalize();
    }

    // Render loop
    for (int i = 0; i < model->nfaces(); i++)
    {
        // Indices of vertices in this face
        std::vector<int> face = model->face(i);
        std::vector<int> tex_face = model->tex_face(i);

        Vec3f v0 = model->vert(face[0]);
        Vec3f v1 = model->vert(face[1]);
        Vec3f v2 = model->vert(face[2]);
        Vec2f uv0 = model->tex_coord(tex_face[0]);
        Vec2f uv1 = model->tex_coord(tex_face[1]);
        Vec2f uv2 = model->tex_coord(tex_face[2]);

        // Face normal for flat shading (backface cull)
        Vec3f normal = ((v2 - v0) ^ (v1 - v0)).normalize();
        if (normal * light_dir <= 0)
            continue; // skip back-facing

        // Flat shading intensity
        float flatIntensity = std::max(0.f, normal * light_dir);
        TGAColor flatColor(
            (unsigned char)(materialColor.r * flatIntensity),
            (unsigned char)(materialColor.g * flatIntensity),
            (unsigned char)(materialColor.b * flatIntensity),
            255);

        // Gouraud intensities
        float i0 = std::max(0.f, vertexNormals[face[0]] * light_dir);
        float i1 = std::max(0.f, vertexNormals[face[1]] * light_dir);
        float i2 = std::max(0.f, vertexNormals[face[2]] * light_dir);

        // Screen-space coords
        Vec3f s0 = world2screen(v0);
        Vec3f s1 = world2screen(v1);
        Vec3f s2 = world2screen(v2);

        // Choose which shading to use by uncommenting:
        // flatShading(s0, s1, s2, image, flatColor, zbuffer);
        // gouraudShading(s0, s1, s2, image, materialColor, zbuffer, i0, i1, i2);
        // phongShading(s0, s1, s2, image, materialColor, zbuffer,
        //               vertexNormals[face[0]], vertexNormals[face[1]], vertexNormals[face[2]],
        //               light_dir);
        addTextures(s0, s1, s2, uv0, uv1, uv2, image, texture, zbuffer);
    }

    // Save final image
    image.flip_vertically();
    image.write_tga_file("assets/outputs/diablo3_pose_output.tga");

    // Cleanup
    delete model;
    delete[] zbuffer;
    return 0;
}
