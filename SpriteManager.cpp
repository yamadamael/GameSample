#include <windows.h>
#include <stdio.h>
#include "bitmap.h"
#include "SpriteManager.h"
#include "Logger.h"

SpriteManager::SpriteManager()
{
    Count = 0;
    Images = new std::map<int, bitmap::Image>();
}

int SpriteManager::ReadBitmap(const char *fileName)
{
    auto index = GetBitmapIndex(fileName);
    if (index >= 0)
    {
        return index;
    }

    FILE *fp;
    errno_t error = fopen_s(&fp, fileName, "r");
    if (error != 0)
    {
        LOG_ERROR("Error: %s could not read.", fileName);
        return -1;
    }

    // ヘッダ部の読み込み
    unsigned char header_buf[HEADERSIZE];
    fread(header_buf, sizeof(unsigned char), HEADERSIZE, fp);

    // 最初の2バイトがBM(Bitmapのファイルの印)であるか
    if (memcmp(header_buf, "BM", 2))
    {
        LOG_ERROR("Error: %s is not Bitmap file.", fileName);
        return -1;
    }

    unsigned int width, height;
    unsigned int color;
    memcpy(&width, header_buf + 18, sizeof(width));
    memcpy(&height, header_buf + 22, sizeof(height));
    memcpy(&color, header_buf + 28, sizeof(color));
    LOG_INFO("width: %d, height: %d, color: %d", width, height, color);

    // 24bitでなければ終了
    if (color != 24)
    {
        LOG_ERROR("Error: %s is not 24bit color image.", fileName);
        return -1;
    }

    // RGB情報は画像の1行分が4byteの倍数でなければならない為合わせている
    int real_width = width * 3 + width % 4;
    LOG_INFO("real_width: %d", real_width);

    // 画像の1行分のRGB情報を取ってくるためのバッファを動的に取得
    unsigned char *bmp_line_data;
    if ((bmp_line_data = (unsigned char *)malloc(sizeof(unsigned char) * real_width)) == NULL)
    {
        LOG_ERROR("Error: Allocation error.");
        return -1;
    }

    // RGB情報を取り込むためのバッファを動的に取得
    bitmap::Image *img;
    if ((img = Create_Image(width, height)) == NULL)
    {
        free(bmp_line_data);
        fclose(fp);
        return -1;
    }

    // BitmapファイルのRGB情報は左下から右へ、下から上へ並んでいる
    for (unsigned int i = 0; i < height; i++)
    {
        fread(bmp_line_data, 1, real_width, fp);
        for (unsigned int j = 0; j < width; j++)
        {
            auto pos = i * width + j;
            img->data[pos].b = bmp_line_data[j * 3];
            img->data[pos].g = bmp_line_data[j * 3 + 1];
            img->data[pos].r = bmp_line_data[j * 3 + 2];
        }
    }

    // 解放
    free(bmp_line_data);
    fclose(fp);

    img->path = fileName;

    auto count = Count++;
    Images->insert(std::make_pair(count, *img));

    return count;
}

// Imageを作成し、RGB情報もwidth*height分だけ動的に取得する
// 成功すればポインタを、失敗すればNullを返す
bitmap::Image *SpriteManager::Create_Image(int width, int height)
{
    bitmap::Image *img;
    // Image構造体メモリ確保
    if ((img = (bitmap::Image *)malloc(sizeof(bitmap::Image))) == NULL)
    {
        LOG_ERROR("Allocation error");
        return NULL;
    }

    // サイズ分のメモリ確保
    if ((img->data = (bitmap::Rgb *)malloc(sizeof(bitmap::Rgb) * width * height)) == NULL)
    {
        LOG_ERROR("Allocation error");
        return NULL;
    }

    img->width = width;
    img->height = height;

    return img;
}

bitmap::Image *SpriteManager::GetBitmap(int index)
{
    return &(*Images)[index];
}

bitmap::Image *SpriteManager::GetBitmap(const char *path)
{
    auto index = GetBitmapIndex(path);
    if (index != -1)
    {
        return GetBitmap(index);
    }

    return nullptr;
}

int SpriteManager::GetBitmapIndex(const char *path)
{
    for (auto itr = Images->begin(); itr != Images->end(); ++itr)
    {
        if (itr->second.path == path)
        {
            return itr->first;
        }
    }

    return -1;
}
