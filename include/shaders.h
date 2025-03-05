// shaders.h
#pragma once
#include "tgaimage.h"
#include "geometry.h"

// Flat shading
void flatShading(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                 TGAImage &image, const TGAColor &color, float *zbuffer);

// Gouraud shading
void gouraudShading(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                    TGAImage &image, const TGAColor &baseColor, float *zbuffer,
                    float i0, float i1, float i2);

// Phong shading
void phongShading(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                  TGAImage &image, const TGAColor &baseColor, float *zbuffer,
                  const Vec3f &n0, const Vec3f &n1, const Vec3f &n2,
                  const Vec3f &lightDir);

// Textured shading
void addTextures(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2,
                 const Vec2f &uv0, const Vec2f &uv1, const Vec2f &uv2,
                 TGAImage &image, TGAImage &texture, float *zbuffer);
