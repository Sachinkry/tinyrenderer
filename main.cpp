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

    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        // Generate a random color for this face
        TGAColor faceColor = generateRandomColor(gen);
        float angle = M_PI / 4;
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            Vec3f v2 = model->vert(face[(j + 2) % 3]);
            // use rotateX, rotateY, rotateZ to rotate the model
            // Vec3f v0 = rotateX(model->vert(face[j]), angle);
            // Vec3f v1 = rotateX(model->vert(face[(j + 1) % 3]), angle);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            int x2 = (v2.x + 1.) * width / 2.;
            int y2 = (v2.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, faceColor);
            triangle(Vec2i(x0, y0), Vec2i(x1, y1), Vec2i(x2, y2), image, blue);
            // std::cout << "Face " << i << ", Edge " << j << ": " << face[j] << " -> " << face[(j + 1) % 3] << std::endl;
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

// int main()
// {
//     TGAImage image(width, height, TGAImage::RGB);

//     Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
//     Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};

//     triangle(t0[0], t0[1], t0[2], image, TGAColor(255, 0, 0, 255));
//     triangle(t1[0], t1[1], t1[2], image, TGAColor(0, 255, 0, 255));
//     triangle(Vec2i(10, 250), Vec2i(400, 400), Vec2i(300, 10), image, white);
//     triangle(Vec2i(264, 370), Vec2i(510, 160), Vec2i(410, 10), image, red);

//     line(10, 10, 100, 100, image, white);

//     image.flip_vertically(); // Flip before saving
//     image.write_tga_file("output.tga");

//     return 0;
// }
