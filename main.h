#pragma once

#include <windows.h>
#include "bitmap.h"
#include "FrameRateCalculator.h"
#include "SpriteManager.h"

class GameObject;

GameObject *go;
FrameRateCalculator *fr;

SpriteManager *spriteManager;

HDC hmdc = NULL;
HBITMAP hBitmap;
RECT rc;

int count = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow);

void Create(HWND hwnd);
void Draw(HWND hwnd);

int GetCpuMax()
{
    SYSTEM_INFO sys;
    GetSystemInfo(&sys);
    return sys.dwNumberOfProcessors;
}
