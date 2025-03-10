#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>
#include <random>
#include "geometry.h"

#pragma pack(push, 1)
struct TGA_Header
{
    char idlength;
    char colormaptype;
    char datatypecode;
    short colormaporigin;
    short colormaplength;
    char colormapdepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    char bitsperpixel;
    char imagedescriptor;
};
#pragma pack(pop)

struct TGAColor
{
    union
    {
        struct
        {
            unsigned char b, g, r, a;
        };
        unsigned char raw[4];
        unsigned int val;
    };
    int bytespp;

    TGAColor() : val(0), bytespp(1)
    {
    }

    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4)
    {
    }

    TGAColor(int v, int bpp) : val(v), bytespp(bpp)
    {
    }

    TGAColor(const TGAColor &c) : val(c.val), bytespp(c.bytespp)
    {
    }

    TGAColor(const unsigned char *p, int bpp) : val(0), bytespp(bpp)
    {
        for (int i = 0; i < bpp; i++)
        {
            raw[i] = p[i];
        }
    }

    TGAColor &operator=(const TGAColor &c)
    {
        if (this != &c)
        {
            bytespp = c.bytespp;
            val = c.val;
        }
        return *this;
    }
};

class TGAImage
{
protected:
    unsigned char *data;
    int width;
    int height;
    int bytespp;

    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out);

public:
    enum Format
    {
        GRAYSCALE = 1,
        RGB = 3,
        RGBA = 4
    };

    TGAImage();
    TGAImage(int w, int h, int bpp);
    TGAImage(const TGAImage &img);
    bool read_tga_file(const char *filename);
    bool write_tga_file(const char *filename, bool rle = true);
    bool flip_horizontally();
    bool flip_vertically();
    bool scale(int w, int h);
    TGAColor get(int x, int y);
    bool set(int x, int y, TGAColor c);
    ~TGAImage();
    TGAImage &operator=(const TGAImage &img);
    int get_width();
    int get_height();
    int get_bytespp();
    unsigned char *buffer();
    void clear();
};

// void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
// void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color, float z0, float z1, float z2, float *zbuffer);
// void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color,
//               float z0, float z1, float z2, float i0, float i1, float i2, float *zbuffer);

// void triangle1(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage &image, TGAColor color, float *zbuffer);

// void trianglePhong(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color,
//                    float z0, float z1, float z2,
//                    const Vec3f &n0, const Vec3f &n1, const Vec3f &n2,
//                    float *zbuffer, const Vec3f &light_dir);

// Vec3f barycentric(Vec3f t0, Vec3f t1, Vec3f t2, Vec3f p);

TGAColor generateRandomColor(std::mt19937 &gen);

#endif //__IMAGE_H__