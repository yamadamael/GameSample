#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bitmap.h"
#include "Logger.h"

// fileNameのBitmapファイルを読み込み、高さと幅、RGB情報をimg構造体に入れる
Image *Read_Bmp(const char *fileName)
{
    Image *img = NULL;

    FILE *fp;
    errno_t error = fopen_s(&fp, fileName, "r");
    if (error != 0)
    {
        LOG_ERROR("Error: %s could not read.", fileName);
        return NULL;
    }

    // ヘッダ部の読み込み
    unsigned char header_buf[HEADERSIZE];
    fread(header_buf, sizeof(unsigned char), HEADERSIZE, fp);

    // 最初の2バイトがBM(Bitmapのファイルの印)であるか
    if (memcmp(header_buf, "BM", 2))
    {
        LOG_ERROR("Error: %s is not Bitmap file.", fileName);
        return NULL;
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
        return NULL;
    }

    // RGB情報は画像の1行分が4byteの倍数でなければならない為合わせている
    int real_width = width * 3 + width % 4;
    LOG_INFO("real_width: %d", real_width);

    // 画像の1行分のRGB情報を取ってくるためのバッファを動的に取得
    unsigned char *bmp_line_data;
    if ((bmp_line_data = (unsigned char *)malloc(sizeof(unsigned char) * real_width)) == NULL)
    {
        LOG_ERROR("Error: Allocation error.");
        return NULL;
    }

    // RGB情報を取り込むためのバッファを動的に取得
    if ((img = Create_Image(width, height)) == NULL)
    {
        free(bmp_line_data);
        fclose(fp);
        return NULL;
    }

    // BitmapファイルのRGB情報は左下から右へ、下から上へ並んでいる
    for (unsigned int i = 0; i < height; i++)
    {
        fread(bmp_line_data, 1, real_width, fp);
        for (unsigned int j = 0; j < width; j++)
        {
            img->data[(height - i - 1) * width + j].b = bmp_line_data[j * 3];
            img->data[(height - i - 1) * width + j].g = bmp_line_data[j * 3 + 1];
            img->data[(height - i - 1) * width + j].r = bmp_line_data[j * 3 + 2];
            // if (bmp_line_data[j * 3] != 255 || bmp_line_data[j * 3 + 1] != 255 || bmp_line_data[j * 3 + 2] != 255)
            // {
            //     LOG_INFO("(%3d, %3d) %02x%02x%02x, ", j, i, bmp_line_data[j * 3], bmp_line_data[j * 3 + 1], bmp_line_data[j * 3 + 2])
            // }
        }
    }

    // 解放
    free(bmp_line_data);
    fclose(fp);

    LOG_INFO("end");
    return img;
}

int Write_Bmp(char *fileName, Image *img)
{
    return 0;
}

int Draw_Bmp(HDC hdc, Image *img, int x, int y)
{
    auto height = img->height;
    auto width = img->width;
    LOG_INFO("%d, %d", height, width);
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            Rgb rgb = img->data[i * width + j];
            SetPixel(hdc, x + j, y + i, RGB(rgb.r, rgb.g, rgb.b));
        }
    }

    return 0;
}

// Imageを作成し、RGB情報もwidth*height分だけ動的に取得する
// 成功すればポインタを、失敗すればNullを返す
Image *Create_Image(int width, int height)
{
    Image *img = NULL;

    // Image構造体メモリ確保
    if ((img = (Image *)malloc(sizeof(Image))) == NULL)
    {
        LOG_ERROR("Allocation error");
        return NULL;
    }

    // サイズ分のメモリ確保
    if ((img->data = (Rgb *)malloc(sizeof(Rgb) * width * height)) == NULL)
    {
        LOG_ERROR("Allocation error");
        return NULL;
    }

    img->width = width;
    img->height = height;

    return img;
}

// Imageを解放する
void Free_Image(Image *img)
{
    free(img->data);
    free(img);
}
