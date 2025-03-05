// shaders.cpp
#include "shaders.h"
#include <algorithm>
#include <cmath>

// Helper: compute barycentric coordinates
Vec3f barycentric(const Vec3f &A, const Vec3f &B, const Vec3f &C, const Vec3f &P)
{
    Vec3f s0 = Vec3f(C.x - A.x, B.x - A.x, A.x - P.x);
    Vec3f s1 = Vec3f(C.y - A.y, B.y - A.y, A.y - P.y);
    Vec3f u = s0 ^ s1;
    if (std::fabs(u.z) > 1e-2) // avoid division by zero
        return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    return Vec3f(-1, 1, 1); // degenerate triangle
}

// Reusable bounding box helper
static void getBoundingBox(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                           int width, int height, int &minX, int &maxX, int &minY, int &maxY)
{
    minX = std::max(0, (int)std::min({t0.x, t1.x, t2.x}));
    maxX = std::min(width - 1, (int)std::max({t0.x, t1.x, t2.x}));
    minY = std::max(0, (int)std::min({t0.y, t1.y, t2.y}));
    maxY = std::min(height - 1, (int)std::max({t0.y, t1.y, t2.y}));
}

// 1) Flat Shading
void flatShading(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                 TGAImage &image, const TGAColor &color, float *zbuffer)
{
    int minX, maxX, minY, maxY;
    getBoundingBox(t0, t1, t2, image.get_width(), image.get_height(), minX, maxX, minY, maxY);

    Vec3f P;
    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc = barycentric(t0, t1, t2, P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            P.z = t0.z * bc.x + t1.z * bc.y + t2.z * bc.z;
            int idx = (int)(P.x + P.y * image.get_width());
            if (zbuffer[idx] < P.z)
            {
                zbuffer[idx] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

// 2) Gouraud Shading
void gouraudShading(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                    TGAImage &image, const TGAColor &baseColor, float *zbuffer,
                    float i0, float i1, float i2)
{
    int minX, maxX, minY, maxY;
    getBoundingBox(t0, t1, t2, image.get_width(), image.get_height(), minX, maxX, minY, maxY);

    Vec3f P;
    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc = barycentric(t0, t1, t2, P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            P.z = t0.z * bc.x + t1.z * bc.y + t2.z * bc.z;
            int idx = (int)(P.x + P.y * image.get_width());
            if (zbuffer[idx] < P.z)
            {
                zbuffer[idx] = P.z;
                float intensity = i0 * bc.x + i1 * bc.y + i2 * bc.z;
                TGAColor color(
                    (unsigned char)(baseColor.r * intensity),
                    (unsigned char)(baseColor.g * intensity),
                    (unsigned char)(baseColor.b * intensity),
                    255);
                image.set(P.x, P.y, color);
            }
        }
    }
}

// 3) Phong Shading
void phongShading(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                  TGAImage &image, const TGAColor &baseColor, float *zbuffer,
                  const Vec3f &n0, const Vec3f &n1, const Vec3f &n2,
                  const Vec3f &lightDir)
{
    // Some constants for the Phong model
    static const float ambient_coeff = 0.2f;
    static const float diffuse_coeff = 0.7f;
    static const float specular_coeff = 0.5f;
    static const float shininess = 10.0f;
    static const Vec3f view_dir(0, 0, -1);

    int minX, maxX, minY, maxY;
    getBoundingBox(t0, t1, t2, image.get_width(), image.get_height(), minX, maxX, minY, maxY);

    Vec3f P;
    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc = barycentric(t0, t1, t2, P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            P.z = t0.z * bc.x + t1.z * bc.y + t2.z * bc.z;
            int idx = (int)(P.x + P.y * image.get_width());
            if (zbuffer[idx] < P.z)
            {
                zbuffer[idx] = P.z;
                // Interpolate normals
                Vec3f normal = n0 * bc.x + n1 * bc.y + n2 * bc.z;
                // Phong lighting
                float ambient = ambient_coeff;
                float n_dot_l = std::max(0.0f, normal.normalize() * lightDir);
                float diffuse = n_dot_l * diffuse_coeff;
                Vec3f reflectDir = (normal * (2.f * n_dot_l) - lightDir).normalize();
                float specular = std::pow(std::max(0.0f, reflectDir * view_dir), shininess) * specular_coeff;
                float intensity = std::min(1.0f, ambient + diffuse + specular);

                TGAColor color(
                    (unsigned char)(baseColor.r * intensity),
                    (unsigned char)(baseColor.g * intensity),
                    (unsigned char)(baseColor.b * intensity),
                    255);
                image.set(P.x, P.y, color);
            }
        }
    }
}

// 4) Textured Shading
void addTextures(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                 const Vec2f &uv0, const Vec2f &uv1, const Vec2f &uv2,
                 TGAImage &image, TGAImage &texture, float *zbuffer)
{
    int minX, maxX, minY, maxY;
    getBoundingBox(t0, t1, t2, image.get_width(), image.get_height(), minX, maxX, minY, maxY);

    Vec3f P;
    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc = barycentric(t0, t1, t2, P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            P.z = t0.z * bc.x + t1.z * bc.y + t2.z * bc.z;
            int idx = (int)(P.x + P.y * image.get_width());
            if (zbuffer[idx] < P.z)
            {
                zbuffer[idx] = P.z;
                // Interpolate UV
                Vec2f uv = uv0 * bc.x + uv1 * bc.y + uv2 * bc.z;
                int tex_x = std::min(texture.get_width() - 1, std::max(0, (int)(uv.x * texture.get_width())));
                int tex_y = std::min(texture.get_height() - 1, std::max(0, (int)(uv.y * texture.get_height())));
                TGAColor color = texture.get(tex_x, tex_y);
                image.set(P.x, P.y, color);
            }
        }
    }
}
