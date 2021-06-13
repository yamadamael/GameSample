#pragma once

#include <chrono>
#include <string>
#include "define.h"

//フレームレート計算クラス
class FrameRateCalculator
{
    long long cnt = 0;
    const int limit = FPS;
    std::wstring fpsStr = L"0fps";
    long long time = currentTime();

    //フレームレートの計算と結果文字列を構築する
    void updateStr();

public:
    //現在時刻を取得する関数
    static long long currentTime();

    //現在時刻を取得する関数
    static long long currentTimeMicro();

    //フレームレート更新メソッド
    std::wstring *update();
};
