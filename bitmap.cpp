#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bitmap.h"
#include "Logger.h"
#include "Matrix4x3.h"
#include "GameManager.h"
#include "SpriteManager.h"

bitmap::bitmap()
{
    imageIndex = -1;
    bmpInfo = *(new BITMAPINFO());
}

// fileNameのBitmapファイルを読み込み、高さと幅、RGB情報をimg構造体に入れる
bitmap::Image *bitmap::Read_Bmp(const char *fileName)
{
    auto instance = GameManager::GetInstance();
    auto sm = instance.spriteManager;
    imageIndex = sm->GetBitmapIndex(fileName);
    bitmap::Image *img = NULL;
    if (imageIndex >= 0)
    {
        img = sm->GetBitmap(imageIndex);
    }
    else
    {
        imageIndex = sm->ReadBitmap(fileName);
        img = sm->GetBitmap(imageIndex);
    }

    // DIBの情報を設定する
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = img->width;
    bmpInfo.bmiHeader.biHeight = img->height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    return img;
}

int bitmap::Write_Bmp(char *fileName)
{
    return 0;
}

int bitmap::Draw_Bmp(HDC hdc, int x, int y)
{
    auto img = Get_Image();
    auto height = img->height;
    auto width = img->width;

    // メモリ確保
    LPDWORD lpPixel;
    lpPixel = (LPDWORD)HeapAlloc(GetProcessHeap(), (DWORD)HEAP_ZERO_MEMORY, (SIZE_T)height * width * 4);

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
    StretchDIBits(hdc, x, y, width, height, 0, 0, width, height, lpPixel, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);

    // メモリ解放
    HeapFree(GetProcessHeap(), 0, lpPixel);

    return 0;
}

int bitmap::Draw_Bmp(HDC hdc, const Matrix4x3 &matrix)
{
    return 0;
}

bitmap::Image *bitmap::Get_Image()
{
    auto instance = GameManager::GetInstance();
    auto sm = instance.spriteManager;
    auto bm = sm->GetBitmap(imageIndex);
    return bm;
}

// Imageを解放する
void bitmap::Free_Image()
{
    auto img = Get_Image();
    free(img->data);
}
