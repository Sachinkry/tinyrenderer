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
// const TGAColor materialColor = TGAColor(255, 255, 255, 255);
const float ambient_coeff = 0.2f;  // Ambient coefficient
const float diffuse_coeff = 0.7f;  // Diffuse coefficient
const float specular_coeff = 0.5f; // Specular coefficient
const float shininess = 10.0f;
// light source
const Vec3f light_dir(0, 0, -1);
const Vec3f view_dir(0, 0, -1);
std::vector<Vec3f> vertexNormals;

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

void triangleGouraud(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor baseColor, float *zbuffer,
                     float i0, float i1, float i2)
{
    int minX = std::max(0, (int)std::min(t0.x, std::min(t1.x, t2.x)));
    int maxX = std::min(image.get_width() - 1, (int)std::max(t0.x, std::max(t1.x, t2.x)));
    int minY = std::max(0, (int)std::min(t0.y, std::min(t1.y, t2.y)));
    int maxY = std::min(image.get_height() - 1, (int)std::max(t0.y, std::max(t1.y, t2.y)));

    Vec3f P;
    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc = barycentric(t0, t1, t2, P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            P.z = t0.z * bc.x + t1.z * bc.y + t2.z * bc.z; // Interpolate z for depth
            int idx = int(P.x + P.y * image.get_width());
            if (zbuffer[idx] < P.z)
            {
                zbuffer[idx] = P.z;
                float intensity = i0 * bc.x + i1 * bc.y + i2 * bc.z; // Interpolate intensity
                TGAColor color = TGAColor(
                    static_cast<unsigned char>(baseColor.r * intensity),
                    static_cast<unsigned char>(baseColor.g * intensity),
                    static_cast<unsigned char>(baseColor.b * intensity),
                    255);
                image.set(P.x, P.y, color);
            }
        }
    }
}

void trianglePhong(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor baseColor, float *zbuffer,
                   Vec3f n0, Vec3f n1, Vec3f n2, Vec3f lightDir)
{
    int minX = std::max(0, (int)std::min(t0.x, std::min(t1.x, t2.x)));
    int maxX = std::min(image.get_width() - 1, (int)std::max(t0.x, std::max(t1.x, t2.x)));
    int minY = std::max(0, (int)std::min(t0.y, std::min(t1.y, t2.y)));
    int maxY = std::min(image.get_height() - 1, (int)std::max(t0.y, std::max(t1.y, t2.y)));

    Vec3f P;
    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc = barycentric(t0, t1, t2, P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            P.z = t0.z * bc.x + t1.z * bc.y + t2.z * bc.z;
            int idx = int(P.x + P.y * image.get_width());
            if (zbuffer[idx] < P.z)
            {
                zbuffer[idx] = P.z;
                Vec3f normal = (n0 * bc.x + n1 * bc.y + n2 * bc.z); // No normalization after interpolation
                float ambient = std::min(1.0f, ambient_coeff);
                float diffuse = std::min(1.0f, std::max(0.0f, normal * lightDir) * diffuse_coeff);
                float n_dot_l = std::max(0.0f, normal * lightDir);
                Vec3f reflectDir = (normal * (2.0f * n_dot_l) - lightDir).normalize();
                float specular = std::min(1.0f, std::pow(std::max(0.0f, reflectDir * view_dir), shininess) * specular_coeff);
                float intensity = ambient + diffuse + specular;
                intensity = std::min(1.0f, intensity);
                TGAColor color = TGAColor(
                    static_cast<unsigned char>(baseColor.r * intensity),
                    static_cast<unsigned char>(baseColor.g * intensity),
                    static_cast<unsigned char>(baseColor.b * intensity),
                    255);
                image.set(P.x, P.y, color);
            }
        }
    }
}

void triangleTexture(Vec3f t0, Vec3f t1, Vec3f t2, Vec2f uv0, Vec2f uv1, Vec2f uv2,
                     TGAImage &image, TGAImage &texture, float *zbuffer)
{
    int minX = std::max(0, (int)std::min(t0.x, std::min(t1.x, t2.x)));
    int maxX = std::min(image.get_width() - 1, (int)std::max(t0.x, std::max(t1.x, t2.x)));
    int minY = std::max(0, (int)std::min(t0.y, std::min(t1.y, t2.y)));
    int maxY = std::min(image.get_height() - 1, (int)std::max(t0.y, std::max(t1.y, t2.y)));

    Vec3f P;
    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc = barycentric(t0, t1, t2, P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            P.z = t0.z * bc.x + t1.z * bc.y + t2.z * bc.z;

            // Interpolate texture coordinates
            Vec2f uv = uv0 * bc.x + uv1 * bc.y + uv2 * bc.z;
            int idx = int(P.x + P.y * image.get_width());
            if (zbuffer[idx] < P.z)
            {
                zbuffer[idx] = P.z;
                int tex_x = static_cast<int>(uv.x * texture.get_width());
                int tex_y = static_cast<int>(uv.y * texture.get_height());
                tex_x = std::max(0, std::min(tex_x, texture.get_width() - 1));
                tex_y = std::max(0, std::min(tex_y, texture.get_height() - 1));
                TGAColor color = texture.get(tex_x, tex_y);
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
        model = new Model("obj/diablo3_pose.obj");
    }

    TGAImage texture;
    texture.read_tga_file("obj/diablo3_pose_nm.tga");
    texture.flip_vertically();

    float *zbuffer = new float[width * height];
    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max())
        ;

    TGAImage image(width, height, TGAImage::RGB);

    // Map to store which faces share each vertex
    std::map<int, std::vector<int>> vertexToFaces;
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            vertexToFaces[face[j]].push_back(i);
        }
    }

    // Resize vertexNormals to hold a normal for each vertex and compute averages
    vertexNormals.resize(model->nverts(), Vec3f(0, 0, 0));
    for (int vertIdx = 0; vertIdx < model->nverts(); vertIdx++)
    {
        const std::vector<int> &faces = vertexToFaces[vertIdx];
        for (int faceIdx : faces)
        {
            std::vector<int> face = model->face(faceIdx);
            Vec3f v0 = model->vert(face[0]);
            Vec3f v1 = model->vert(face[1]);
            Vec3f v2 = model->vert(face[2]);
            Vec3f edge1 = v1 - v0;
            Vec3f edge2 = v2 - v0;
            Vec3f normal = edge2 ^ edge1; // Cross product for face normal
            normal.normalize();
            vertexNormals[vertIdx] = vertexNormals[vertIdx] + normal;
        }
        vertexNormals[vertIdx].normalize(); // Normalize the average normal
    }

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        std::vector<int> tex_face = model->tex_face(i);

        // Get 3D coordinates of the triangle's vertices
        Vec3f v0 = model->vert(face[0]);
        Vec3f v1 = model->vert(face[1]);
        Vec3f v2 = model->vert(face[2]);
        // Get texture coordinates using texture indices
        Vec2f uv0 = model->tex_coord(tex_face[0]);
        Vec2f uv1 = model->tex_coord(tex_face[1]);
        Vec2f uv2 = model->tex_coord(tex_face[2]);

        // Compute face normal
        Vec3f edge1 = v1 - v0;
        Vec3f edge2 = v2 - v0;
        Vec3f normal = edge2 ^ edge1; // Cross product for CCW winding
        normal.normalize();

        // Backface culling using view_dir
        float facing = normal * light_dir;
        if (facing <= 0)
            continue; // Skip back-facing triangles

        // Compute intensity using light_dir; for flat shading
        float intensity = std::max(0.f, normal * light_dir);

        // Scale materialColor by intensity
        TGAColor color = TGAColor(
            static_cast<unsigned char>(materialColor.r * intensity),
            static_cast<unsigned char>(materialColor.g * intensity),
            static_cast<unsigned char>(materialColor.b * intensity),
            255);

        // Compute intensity for each vertex using vertex normals
        float intensity0 = std::max(0.f, vertexNormals[face[0]] * light_dir);
        float intensity1 = std::max(0.f, vertexNormals[face[1]] * light_dir);
        float intensity2 = std::max(0.f, vertexNormals[face[2]] * light_dir);

        Vec3f pts[3];
        // std::cout << pts[0] << std::endl;
        for (int j = 0; j < 3; j++)
        {
            pts[j] = world2screen(model->vert(face[j]));

            // std::cout << pts[i] << std::endl;
        }
        Vec3f normLightDir = light_dir;
        normLightDir.normalize();

        // triangle1(pts[0], pts[1], pts[2], image, color, zbuffer); // flat shading
        // triangleGouraud(pts[0], pts[1], pts[2], image, materialColor, zbuffer, intensity0, intensity1, intensity2);
        // trianglePhong(pts[0], pts[1], pts[2], image, materialColor, zbuffer, vertexNormals[face[0]], vertexNormals[face[1]], vertexNormals[face[2]], normLightDir);
        triangleTexture(pts[0], pts[1], pts[2], uv0, uv1, uv2, image, texture, zbuffer);
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("obj/diablo3_pose_diffuse_texture.tga");
    delete model;
    return 0;
}