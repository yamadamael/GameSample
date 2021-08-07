#pragma once

#include <map>
#include "bitmap.h"

class Matrix4x3;

class SpriteManager
{
public:
    int Count;
    std::map<int, bitmap::Image> *Images;

    SpriteManager();

    int ReadBitmap(const char *path);
    bitmap::Image *Create_Image(int width, int height);
    bitmap::Image *GetBitmap(int index);
    bitmap::Image *GetBitmap(const char *path);
    int GetBitmapIndex(const char *path);
};
