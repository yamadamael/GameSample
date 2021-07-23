#pragma once

#include <math.h>

// Piといくつかの倍数用のグローバル定数
const float kPi = 3.14159265f;
const float k2Pi = kPi * 2;
const float kPiOver2 = kPi / 2;
const float k1OverPi = 1 / kPi;
const float k1Over2Pi = 1 / k2Pi;

// 適切に2Piの倍数を加えることで角度を-pi~piの範囲にラップする
extern float wrapPi(float theta);

// 安全な逆三角形
extern float safeAcos(float x);

// ある角度のsinとcosを計算する
// プラットフォームによっては両方の値が必要なことが分かっている場合、
// 2つの値を別に計算するよりも速く計算することができる
inline void sinCos(float *returnSin, float *returnCos, float theta)
{
    *returnSin = sinf(theta);
    *returnCos = cosf(theta);
}
