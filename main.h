#pragma once

#include <windows.h>
#include "bitmap.h"

bitmap *bmp;

int count = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow);

void Create(HWND hwnd);
void Draw(HWND hwnd);
