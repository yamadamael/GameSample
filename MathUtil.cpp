#include <math.h>
#include "MathUtil.h"

// 適切に2Piの倍数を加えることで角度を-Pi~Piの範囲にラップする
extern float wrapPi(float theta)
{
    theta += kPi;
    theta -= floorf(theta * k1Over2Pi) * k2Pi;
    theta -= kPi;
    return theta;
}

// acos(x)と同じ
// ただし、xが範囲外の場合、最も近い有効な値にクランプ処理される
// 返される値は、範囲0...Piであり、これは標準のacos()と同じである
extern float safeAcos(float x)
{
    // 制限条件をチェックする
    if (x <=-1.0f)
    {
        return kPi;
    }
    if (x >= 1.0f)
    {
        return 0.0f;
    }
    return acosf(x);
}
