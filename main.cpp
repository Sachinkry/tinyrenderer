#include <vector>
#include <cmath>
#include <random>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
// light source
const Vec3f light_dir(0, 0, -1);

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
    // Set up random number generation
    std::random_device rd;  // Seed generator
    std::mt19937 gen(rd()); // Mersenne Twister engine

    // Initialize image and z-buffer
    TGAImage image(width, height, TGAImage::RGB); // 800x800 RGB canvas
    float *zbuffer = new float[width * height];   // Depth buffer for z values
    std::fill(zbuffer, zbuffer + width * height, std::numeric_limits<float>::max());

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        // Generate a random color for this face
        // TGAColor faceColor = generateRandomColor(gen);
        float angle = M_PI / 4;
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            Vec3f v2 = model->vert(face[(j + 2) % 3]);

            // calculate normal
            Vec3f n = (v2 - v0) ^ (v1 - v0);
            n.normalize();
            float facing = n * light_dir;
            // std::cout << "Face " << i << ": Normal (" << n.x << ", " << n.y << ", " << n.z << "), Facing: " << facing << std::endl; // Debug
            // if (facing < 0)
            //     continue;
            float intensity = std::max(0.f, facing);
            const int r = 255, g = 255, b = 255; // for white-black shading
            TGAColor faceColor = TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255);

            // use rotateX, rotateY, rotateZ to rotate the model
            // Vec3f v0 = rotateX(model->vert(face[j]), angle);
            // Vec3f v1 = rotateX(model->vert(face[(j + 1) % 3]), angle);
            rotateX(v0, angle);
            rotateX(v1, angle);
            rotateX(v2, angle);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            int x2 = (v2.x + 1.) * width / 2.;
            int y2 = (v2.y + 1.) * height / 2.;
            // line(x0, y0, x1, y1, image, faceColor);
            if (intensity > 0)
            {
                triangle(Vec2i(x0, y0), Vec2i(x1, y1), Vec2i(x2, y2), image, faceColor, v0.z, v1.z, v2.z, zbuffer);
                // triangle(Vec2i(x0, y0), Vec2i(x1, y1), Vec2i(x2, y2), image, faceColor);
            }

            // std::cout << "Face " << i << ", Edge " << j << ": " << face[j] << " -> " << face[(j + 1) % 3] << std::endl;
        }
    }

    image.flip_vertically(); // shift origin to bottom left
    image.write_tga_file("output.tga");
    delete[] zbuffer;
    delete model;
    return 0;
}
