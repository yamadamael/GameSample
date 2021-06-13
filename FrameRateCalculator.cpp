#include <chrono>
#include <string>
#include "Logger.h"
#include "FrameRateCalculator.h"

//現在時刻を取得する関数
long long FrameRateCalculator::currentTime()
{
    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

//現在時刻を取得する関数
long long FrameRateCalculator::currentTimeMicro()
{
    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

//フレームレートの計算と結果文字列を構築する
void FrameRateCalculator::updateStr()
{
    //fpsを計算し、文字列として保持する
    long long end = currentTime();
    double fpsResult = (double)(1000) / (end - time) * cnt;
    LOG_INFO("%lf, %lld, %lld, %lld", fpsResult, end, time, end - time);
    time = end;
    fpsStr = std::to_wstring(fpsResult) + L"fps";
    cnt = 0;
}

//フレームレート更新メソッド
std::wstring *FrameRateCalculator::update()
{
    cnt++;
    //規定フレーム数になったらフレームレートの更新
    if (limit <= cnt)
    {
        updateStr();
    }
    return &fpsStr;
}
