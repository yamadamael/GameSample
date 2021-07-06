#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bitmap.h"
#include "Logger.h"

bitmap::bitmap()
{
    img = NULL;
    bmpInfo = new BITMAPINFO();
}

// fileNameのBitmapファイルを読み込み、高さと幅、RGB情報をimg構造体に入れる
bitmap::Image *bitmap::Read_Bmp(const char *fileName)
{
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
            auto pos = i * width + j;
            img->data[pos].b = bmp_line_data[j * 3];
            img->data[pos].g = bmp_line_data[j * 3 + 1];
            img->data[pos].r = bmp_line_data[j * 3 + 2];
        }
    }

    // 解放
    free(bmp_line_data);
    fclose(fp);

    // DIBの情報を設定する
    bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo->bmiHeader.biWidth = width;
    bmpInfo->bmiHeader.biHeight = height;
    bmpInfo->bmiHeader.biPlanes = 1;
    bmpInfo->bmiHeader.biBitCount = 32;
    bmpInfo->bmiHeader.biCompression = BI_RGB;

    LOG_INFO("end");
    return img;
}

int bitmap::Write_Bmp(char *fileName)
{
    return 0;
}

int bitmap::Draw_Bmp(HDC hdc, int x, int y)
{
    auto height = img->height;
    auto width = img->width;

    // メモリ確保
    LPDWORD lpPixel;
    lpPixel = (LPDWORD)HeapAlloc(GetProcessHeap(), (DWORD)HEAP_ZERO_MEMORY, height * width * 4);

    for (unsigned int i = 0; i < height; i++)
    {
        auto w = i * width;
        for (unsigned int j = 0; j < width; j++)
        {
            bitmap::Rgb rgb = img->data[w + j];
            int color = (rgb.r << (8 * 2)) | (rgb.g << (8 * 1)) | rgb.b;
            lpPixel[w + j] = color;
        }
    }

    // 描画
    StretchDIBits(hdc, x, y, width, height, 0, 0, width, height, lpPixel, bmpInfo, DIB_RGB_COLORS, SRCCOPY);

    // メモリ解放
    HeapFree(GetProcessHeap(), 0, lpPixel);

    return 0;
}

// Imageを作成し、RGB情報もwidth*height分だけ動的に取得する
// 成功すればポインタを、失敗すればNullを返す
bitmap::Image *bitmap::Create_Image(int width, int height)
{
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

bitmap::Image *bitmap::Get_Image()
{
    return img;
}

// Imageを解放する
void bitmap::Free_Image()
{
    free(img->data);
    free(img);
}
