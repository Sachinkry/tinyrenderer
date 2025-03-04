#include <vector>
#include <cmath>
#include <random>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>
#include <map>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const TGAColor materialColor = TGAColor(139, 69, 19, 255);
// const TGAColor color = TGAColor(255, 255, 255, 255);

// light source
const Vec3f light_dir(1, 1, -1);
const Vec3f view_dir(0, 0, -1);

Model *model = NULL;
const int width = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++)
    {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
    }
}

// int main(int argc, char **argv)
// {
//     if (2 == argc)
//     {
//         model = new Model(argv[1]);
//     }
//     else
//     {
//         model = new Model("obj/african_head.obj");
//     }

//     std::random_device rd;
//     std::mt19937 gen(rd());

//     TGAImage image(width, height, TGAImage::RGB);
//     float *zbuffer = new float[width * height];
//     std::fill(zbuffer, zbuffer + width * height, std::numeric_limits<float>::max());

//     std::vector<Vec3f> vertexNormals(model->nverts(), Vec3f(0, 0, 0));
//     std::map<int, std::vector<int>> vertexToFaces;

//     for (int i = 0; i < model->nfaces(); i++)
//     {
//         std::vector<int> face = model->face(i);
//         for (int j = 0; j < 3; j++)
//         {
//             vertexToFaces[face[j]].push_back(i);
//         }
//     }

//     for (int i = 0; i < model->nfaces(); i++)
//     {
//         std::vector<int> face = model->face(i);
//         Vec3f v0 = model->vert(face[0]);
//         Vec3f v1 = model->vert(face[1]);
//         Vec3f v2 = model->vert(face[2]);
//         // Vec3f normal = (v1 - v0) ^ (v2 - v0); // Standard CCW order
//         Vec3f normal = (v2 - v0) ^ (v1 - v0); // Reverse CW order
//         normal.normalize();
//         vertexNormals[face[0]] = vertexNormals[face[0]] + normal;
//         vertexNormals[face[1]] = vertexNormals[face[1]] + normal;
//         vertexNormals[face[2]] = vertexNormals[face[2]] + normal;
//     }
//     for (int i = 0; i < model->nverts(); i++)
//     {
//         vertexNormals[i].normalize();
//     }

//     Vec3f norm_light = light_dir;
//     norm_light.normalize();

//     for (int i = 0; i < model->nfaces(); i++)
//     {
//         std::vector<int> face = model->face(i);
//         Vec3f v0 = model->vert(face[0]);
//         Vec3f v1 = model->vert(face[1]);
//         Vec3f v2 = model->vert(face[2]);

//         Vec3f edge1 = v1 - v0;
//         Vec3f edge2 = v2 - v0;
//         Vec3f normal = edge2 ^ edge1;
//         // Vec3f normal = edge1 ^ edge2;
//         normal.normalize();

//         // Use view_dir for culling, light_dir for intensity
//         float facing = normal * view_dir; // Correct culling direction
//         // std::cout << "Face " << i << ": Normal (" << normal.x << ", " << normal.y << ", " << normal.z << "), Facing: " << facing << std::endl;
//         if (facing <= 0)
//             continue; // Cull back faces

//         float n = normal * light_dir;
//         float intensity0 = std::max(0.f, vertexNormals[face[0]] * light_dir);
//         float intensity1 = std::max(0.f, vertexNormals[face[1]] * light_dir);
//         float intensity2 = std::max(0.f, vertexNormals[face[2]] * light_dir);

//         int x0 = (v0.x + 1.) * width / 2.;
//         int y0 = (v0.y + 1.) * height / 2.;
//         int x1 = (v1.x + 1.) * width / 2.;
//         int y1 = (v1.y + 1.) * height / 2.;
//         int x2 = (v2.x + 1.) * width / 2.;
//         int y2 = (v2.y + 1.) * height / 2.;

//         // Use per-pixel Phong shading by passing vertex normals:
//         trianglePhong(
//             Vec2i(x0, y0), Vec2i(x1, y1), Vec2i(x2, y2),
//             image, materialColor,
//             v0.z, v1.z, v2.z,
//             vertexNormals[face[0]], vertexNormals[face[1]], vertexNormals[face[2]],
//             zbuffer, norm_light);
//         if (intensity0 > 0 || intensity1 > 0 || intensity2 > 0)
//         {
//             // triangle(Vec2i(x0, y0), Vec2i(x1, y1), Vec2i(x2, y2), image, materialColor,
//             //  v0.z, v1.z, v2.z, intensity0, intensity1, intensity2, zbuffer);
//             // line(x0, y0, x1, y1, image, white);
//             // line(x1, y1, x2, y2, image, white);
//             // line(x2, y2, x0, y0, image, white);
//         }
//     }

//     image.flip_vertically();
//     image.write_tga_file("output.tga");
//     delete[] zbuffer;
//     delete model;
//     return 0;
// }

void triangle1(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor color, float *zbuffer)
{
    int minX = std::min(t0.x, std::min(t1.x, t2.x));
    int maxX = std::max(t0.x, std::max(t1.x, t2.x));
    int minY = std::min(t0.y, std::min(t1.y, t2.y));
    int maxY = std::max(t0.y, std::max(t1.y, t2.y));
    minX = std::max(0, minX);
    maxX = std::min(image.get_width() - 1, maxX);
    minY = std::max(0, minY);
    maxY = std::min(image.get_height() - 1, maxY);

    Vec3f P;

    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc = barycentric(t0, t1, t2, P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            P.z = 0;
            // for (int i=0; i<3; i++) P.z +=
            P.z += t0.z * bc.x + t1.z * bc.y + t2.z * bc.z;

            int idx = int(P.x + P.y * image.get_width());
            if (zbuffer[idx] < P.z)
            {

                zbuffer[idx] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

Vec3f world2screen(Vec3f v)
{
    return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

int main(int argc, char **argv)
{
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("obj/african_head.obj");
    }

    float *zbuffer = new float[width * height];
    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max())
        ;

    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);

        // Get 3D coordinates of the triangle's vertices
        Vec3f v0 = model->vert(face[0]);
        Vec3f v1 = model->vert(face[1]);
        Vec3f v2 = model->vert(face[2]);

        // Compute face normal
        Vec3f edge1 = v1 - v0;
        Vec3f edge2 = v2 - v0;
        Vec3f normal = edge2 ^ edge1; // Cross product for CCW winding
        normal.normalize();

        // Backface culling using view_dir
        float facing = normal * light_dir;
        if (facing <= 0)
            continue; // Skip back-facing triangles

        // Compute intensity using light_dir
        float intensity = std::max(0.f, normal * light_dir);

        // Scale materialColor by intensity
        TGAColor color = TGAColor(
            static_cast<unsigned char>(materialColor.r * intensity),
            static_cast<unsigned char>(materialColor.g * intensity),
            static_cast<unsigned char>(materialColor.b * intensity),
            255);

        Vec3f pts[3];
        // std::cout << pts[0] << std::endl;
        for (int j = 0; j < 3; j++)
        {
            pts[j] = world2screen(model->vert(face[j]));

            // std::cout << pts[i] << std::endl;
        }

        triangle1(pts[0], pts[1], pts[2], image, color, zbuffer);
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}