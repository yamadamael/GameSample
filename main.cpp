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
#include "GameObject.h"
#include "GameManager.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "EulerAngles.h"
#include "MathUtil.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        Create(hwnd);
        return 0;
    case WM_DESTROY:
        go->Destroy();

        DeleteDC(hmdc);
        hmdc = NULL;

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
    spriteManager = new SpriteManager();

    go = new GameObject("Root");
    go->SetPosition(new Vector3(100, 100, 0));
    go->ReadBmp("bmp1.bmp");

    GameObject *child = new GameObject("Child1");
    child->SetPosition(new Vector3(100, 100, 0));
    auto rotation = new Quaternion();
    rotation->setToRotateInertialToObject(EulerAngles(0, 0, kPi));
    child->SetRotation(rotation);
    child->ReadBmp("bmp1.bmp");
    child->SetParent(go);

    fr = new FrameRateCalculator();

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
    HBRUSH hBrush, hOldBrush;

    // ウィンドウのデバイスコンテキストを取得
    hdc = BeginPaint(hwnd, &ps);

    // 背景
    hBrush = GetSysColorBrush(COLOR_WINDOW);
    hOldBrush = (HBRUSH)SelectObject(hmdc, hBrush);
    PatBlt(hmdc, 0, 0, rc.right, rc.bottom, PATCOPY);
    SelectObject(hmdc, hOldBrush);

    // ここからDCへの描画
    auto count = 1;
    for (auto i = 0; i < count; i++)
    {
        go->Draw(hmdc);
    }

    //fps描画
    std::wstring *fpsStr = fr->update();
    TextOut(hmdc, 10, 30, fpsStr->c_str(), (int)fpsStr->size());

    BitBlt(hdc, 0, 0, rc.right, rc.bottom, hmdc, 0, 0, SRCCOPY);

    EndPaint(hwnd, &ps);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow)
{
    LOG_INFO("main start");

    // 使用CPUを固定
    HANDLE process = GetCurrentProcess();
    DWORD_PTR processAffinityMask = 1;
    bool success = SetProcessAffinityMask(process, processAffinityMask);
    LOG_INFO("cpu%d: %s", processAffinityMask, success ? "true" : "false");
    LOG_INFO("cpu count: %d", GetCpuMax());

    HWND hwnd;
    MSG msg = {0};
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
        TEXT("KITTY"), TEXT("GameSample"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return -1;

    //メッセージループ
    int cnt = 0;
    HDC hdc = GetDC(hwnd);

    //60fpsで動作させる
    int fps = FPS;

    //現在時刻を取得(1秒=1000000)
    long long end = FrameRateCalculator::currentTimeMicro();

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
            end = FrameRateCalculator::currentTimeMicro();
            if (end < next)
            {
                //更新時間まで待機
                auto waitFrame = (next - end);
                // std::this_thread::sleep_for(std::chrono::microseconds(waitFrame));
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

    ReleaseDC(hwnd, hdc);
    return (int)msg.wParam;
}
