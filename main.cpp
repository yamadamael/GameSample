#include <windows.h>
#include "Logger.h"
#include "bitmap.h"
#include "resource.h"

Image *img;
int posX;
HDC hmdc = NULL;
HBITMAP hBitmap;
RECT rc;
HINSTANCE hinst;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    PAINTSTRUCT ps;

    static HBITMAP hb;
    static HDC mhdc;
    static int width, height;

    switch (msg)
    {
    case WM_CREATE:
        // ビットマップ直読み込み
        hb = LoadBitmap(hinst, MAKEINTRESOURCE(BMP1));
        BITMAP bp;
        GetObject(hb, (int)sizeof(BITMAP), &bp);
        width = bp.bmWidth;
        height = bp.bmHeight;
        mhdc = CreateCompatibleDC(NULL);
        SelectObject(mhdc, hb);

        img = Read_Bmp("bmp1.bmp");

        // ウィンドウのデバイスコンテキストを取得
        hdc = GetDC(hwnd);

        // ウィンドウのデバイスコンテキストに関連付けられたメモリDCを作成
        hmdc = CreateCompatibleDC(hdc);
        // デバイスコンテキストと互換のあるビットマップを作成
        GetClientRect(hwnd, &rc);
        hBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);

        // メモリDCとビットマップを関連付け
        SelectObject(hmdc, hBitmap);

        // ウィンドウのデバイスコンテキストを解放
        ReleaseDC(hwnd, hdc);

        return 0;
    case WM_DESTROY:
        Free_Image(img);

        // メモリDCとビットマップオブジェクトの削除
        DeleteDC(hmdc);
        DeleteObject(hBitmap);
        DeleteObject(hb);

        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        /*
        LOG_INFO("%d", msg);

        hdc = BeginPaint(hwnd, &ps);
        // for (auto iCount = 0; iCount < 1000; iCount++)
        //     SetPixel(hdc, 10 + iCount, 10, 0xFF);
        // for (auto i = 0; i < 200; i++)
        //     SetPixel(hdc, 50 + i, 10, RGB(255, 0, 0));
        LOG_INFO("%d", posX);
        Draw_Bmp(hdc, img, 100 + posX, 100);
        EndPaint(hwnd, &ps);
        */

        // ウィンドウのデバイスコンテキストを取得
        hdc = BeginPaint(hwnd, &ps);

        // ここからメモリDCへの描画
        // posX += 10;
        // Draw_Bmp(hmdc, img, 100 + posX, 100);

        // メモリDCから画像を転送
        // BitBlt(hdc, 0, 0, rc.right, rc.bottom, hmdc, 0, 0, SRCCOPY);

        BitBlt(hdc, 0, 0, width, height, mhdc, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);

        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow)
{
    LOG_INFO("main start");
    posX = 0;
    hinst = hInstance;

    HWND hwnd;
    MSG msg;
    WNDCLASS winc;

    winc.style = CS_HREDRAW | CS_VREDRAW;
    winc.lpfnWndProc = WndProc;
    winc.cbClsExtra = winc.cbWndExtra = 0;
    winc.hInstance = hInstance;
    winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winc.hCursor = LoadCursor(NULL, IDC_ARROW);
    winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winc.lpszMenuName = NULL;
    winc.lpszClassName = TEXT("KITTY");

    if (!RegisterClass(&winc))
        return -1;

    hwnd = CreateWindow(
        TEXT("KITTY"), TEXT("Kitty on your lap"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return -1;

    if (1)
    {
    }
    else
    {
    }

    while (GetMessage(&msg, NULL, 0, 0))
    {
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
