#include <windows.h>
#include "Logger.h"
#include "bitmap.h"

Image *img;
int posX;
HDC hmdc = NULL;
HBITMAP hBitmap;
RECT rc;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (msg)
    {
    case WM_CREATE:
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

        PostQuitMessage(0);
        return 0;
    case WM_PAINT:

        PAINTSTRUCT ps;
        HPEN hPen, hOldPen;
        HBRUSH hBrush, hOldBrush;

        // ウィンドウのデバイスコンテキストを取得
        HDC hdc = BeginPaint(hwnd, &ps);

        // ここからメモリDCへの描画
        // 背景を塗りつぶし
        hBrush = GetSysColorBrush(COLOR_WINDOW);

        hOldBrush = (HBRUSH)SelectObject(hBrush);
        PatBlt(hmdc, 0, 0, rc.right, rc.bottom, PATCOPY);

        // ペンとブラシの作成
        hBrush = CreateSolidBrush(RGB(0xff, 0x88, 0x88));
        SelectObject(hmdc, hBrush);
        hPen = CreatePen(PS_SOLID, 1, RGB(0x00, 0x00, 0xff));
        hOldPen = (HPEN)SelectObject(hmdc, hPen);
        // 四角形描画
        Rectangle(hmdc, 10, 10, 110, 110);

        // ペンとブラシの作成
        hBrush = CreateSolidBrush(RGB(0xff, 0xff, 0x88));
        SelectObject(hmdc, hBrush);
        hPen = CreatePen(PS_SOLID, 1, RGB(0xff, 0x00, 0x00));
        SelectObject(hmdc, hPen);
        // 円を描画
        Ellipse(hmdc, 130, 130, 230, 230);

        // ウィンドウのデバイスコンテキストに円を描画
        Ellipse(hdc, 100, 100, 300, 300);

        // メモリDCから画像を転送
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, hmdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);

        // オブジェクトを元に戻す
        SelectObject(hmdc, hOldBrush);
        SelectObject(hmdc, hOldPen);

        // 作成したオブジェクトの削除
        DeleteObject(hBrush);
        DeleteObject(hPen);

        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow)
{
    LOG_INFO("main start");
    posX = 0;

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
