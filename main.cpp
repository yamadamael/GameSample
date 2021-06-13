#include <thread>
#include <chrono>
#include <string>
#include <functional>
#include <windows.h>
#include "define.h"
#include "main.h"
#include "Logger.h"
#include "resource.h"
#include "FrameRateCalculator.h"

FrameRateCalculator fr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        Create(hwnd);
        return 0;
    case WM_DESTROY:
        bmp->Free_Image();

        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        Draw(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void Create(HWND hwnd)
{
    bmp = new bitmap();
    bmp->Read_Bmp("bmp1.bmp");

    // 裏画面
    {
        // ウィンドウのデバイスコンテキストを取得
        HDC hdc = GetDC(hwnd);

        // ウィンドウのデバイスコンテキストに関連付けられたメモリDCを作成
        hmdc = CreateCompatibleDC(hdc);

        // デバイスコンテキストと互換のあるビットマップを作成
        GetClientRect(hwnd, &rc);

        hBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);

        // メモリDCとビットマップを関連付け
        SelectObject(hmdc, hBitmap);

        // ウィンドウのデバイスコンテキストを解放
        ReleaseDC(hwnd, hdc);

        DeleteObject(hBitmap);
    }
}

void Draw(HWND hwnd)
{
    HDC hdc;
    PAINTSTRUCT ps;

    // ウィンドウのデバイスコンテキストを取得
    hdc = BeginPaint(hwnd, &ps);

    // ここからDCへの描画
    auto count = 100;
    for (auto i = 0; i < count; i++)
    {
        bmp->Draw_Bmp(hmdc, 100, 100);
    }

    //fps描画
    std::wstring *fpsStr = fr.update();
    TextOut(hmdc, 10, 30, fpsStr->c_str(), (int)fpsStr->size());

    BitBlt(hdc, 0, 0, rc.right, rc.bottom, hmdc, 0, 0, SRCCOPY);

    EndPaint(hwnd, &ps);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow)
{
    LOG_INFO("main start");

    HWND hwnd;
    MSG msg = {0};
    WNDCLASS winc;

    for (auto i = 0; i < 100; i++)
    {
        // std::this_thread::sleep_for(std::chrono::microseconds(1000 * 1000));
        // LOG_INFO("sleep, %05d", i);
    }

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

    //現在時刻をマイクロ秒で取得
    std::function<long long(void)> currentTimeMicro = []() {
        std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
    };

    //fps計算用オブジェクト
    FrameRateCalculator fr;

    //メッセージループ
    // MSG msg = {};
    int cnt = 0;
    HDC hdc = GetDC(hwnd);

    //60fpsで動作させる
    int fps = FPS;

    //現在時刻を取得(1秒=1000000)
    long long end = currentTimeMicro();

    //次の更新時間を計算(1秒/フレームレート)
    long long next = end + (1000 * 1000 / fps);
    while (true)
    {
        //メッセージを取得したら1(true)を返し取得しなかった場合は0(false)を返す
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                //終了メッセージが来たらゲームループから抜ける
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            //ゲームの処理を記述
            //DirectXの描画処理などもここに記述する
            //今回はGDIでカウントアップを描画する処理で動作テスト
            // cnt++;
            // std::wstring str = std::to_wstring(cnt);
            // TextOut(hdc, 10, 10, str.c_str(), (int)str.size());

            //重い処理があったとする
            // std::this_thread::sleep_for(std::chrono::microseconds(100));
            InvalidateRect(hwnd, NULL, false); //領域無効化
            UpdateWindow(hwnd);                //再描画命令

            //できるだけ60fpsになるようにスレッド待機
            end = currentTimeMicro();
            if (end < next)
            {
                //更新時間まで待機
                auto waitFrame = (next - end);
                std::this_thread::sleep_for(std::chrono::microseconds(waitFrame));
                // LOG_INFO("%4lld, %lld, %lld, %lld", waitFrame, next, end, next - end);

                //次の更新時間を計算(1秒/フレームレート加算)
                next += (1000 * 1000 / fps);
            }
            else
            {
                // LOG_INFO("xxxx, %lld, %lld, %lld", next, end, next - end);

                //更新時間を過ぎた場合は現在時刻から次の更新時間を計算
                next = end + (1000 * 1000 / fps);
            }
        }
    }

    // int cnt = 0;
    // HDC hdc = GetDC(hwnd);
    // while (msg.message != WM_QUIT)
    // {
    //     // メッセージを取得したら1を返し、取得しなかった場合は、0を返す
    //     if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    //     {
    //         TranslateMessage(&msg);
    //         DispatchMessage(&msg);
    //         // LOG_INFO("%s, %d", "こんちわ", msg.message);
    //     }
    //     else
    //     {
    //         // LOG_INFO("%s", "わちんこ");
    //         // cnt++;
    //         // TCHAR buf[10];
    //         // wsprintf(buf, L"%d", cnt);
    //         // TextOut(hdc, 10, 10, buf, sizeof(buf));
    //         // std::this_thread::sleep_for(std::chrono::milliseconds(17));
    //     }
    // }

    ReleaseDC(hwnd, hdc);
    return msg.wParam;
}
