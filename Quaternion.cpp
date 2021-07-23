#include <assert.h>
#include <math.h>

#include "Quaternion.h"
#include "MathUtil.h"
#include "Vector3.h"
#include "EulerAngles.h"

// グローバルな恒等四元数
const Quaternion kQuaternionIdentity = { 1.0f, 0.0f, 0.0f, 0.0f };

// 指定された軸周りの回転を四元数にセットアップする
void Quaternion::setToRotateAboutX(float theta)
{
    float thetaOver2 = theta * 0.5f;
    w = cos(thetaOver2);
    x = sin(thetaOver2);
    y = 0;
    z = 0;
}
void Quaternion::setToRotateAboutY(float theta)
{
    float thetaOver2 = theta * 0.5f;
    w = cos(thetaOver2);
    x = 0;
    y = sin(thetaOver2);
    z = 0;
}
void Quaternion::setToRotateAboutZ(float theta)
{
    float thetaOver2 = theta * 0.5f;
    w = cos(thetaOver2);
    x = 0;
    y = 0;
    z = sin(thetaOver2);
}
void Quaternion::setToRotateAboutAxis(const Vector3 &axis, float theta)
{
    float thetaOver2 = theta * 0.5f;
    float sinThetaOver2 = sin(thetaOver2);
    w = cos(thetaOver2);
    x = axis.x * sinThetaOver2;
    y = axis.y * sinThetaOver2;
    z = axis.z * sinThetaOver2;
}

void Quaternion::setToRotateObjectToInertial(const EulerAngles &orientation)
{
    // 半分の角のsinとcosを計算する
    float sh, sp, sb;
    float ch, cp, cb;
    sinCos(&sh, &ch, orientation.heading * 0.5f);
    sinCos(&sp, &cp, orientation.pitch * 0.5f);
    sinCos(&sb, &cb, orientation.bank * 0.5f);

    // 値を計算する
    w = ch * cp * cb + sh * sp * sb;
    x = ch * sp * cb + sh * cp * sb;
    y = sh * cp * cb - ch * sp + sb;
    z = ch * cp * sb - sh * sp * cb;
}
void Quaternion::setToRotateInertialToObject(const EulerAngles &orientation)
{
    // 半分の角のsinとcosを計算する
    float sh, sp, sb;
    float ch, cp, cb;
    sinCos(&sh, &ch, orientation.heading * 0.5f);
    sinCos(&sp, &cp, orientation.pitch * 0.5f);
    sinCos(&sb, &cb, orientation.bank * 0.5f);

    // 値を計算する
    w = ch * cp * cb + sh * sp * sb;
    x = -ch * sp * cb - sh * cp * sb;
    y = ch * sp * sb - sh * cp * cb;
    z = sh * sp * cb - ch * cp * sb;
}

// 外積
Quaternion Quaternion::operator*(const Quaternion &a) const
{
    Quaternion result;
    result.w = w * a.w - x * a.x - y * a.y - z * a.z;
    result.x = w * a.x + x * a.w + y * a.z - z * a.y;
    result.y = w * a.y + y * a.w + z * a.x - x * a.z;
    result.z = w * a.z + z * a.w + x * a.y - y * a.x;
    return result;
}

// 外積と代入の組み合わせ:C++の慣習による
Quaternion &Quaternion::operator*=(const Quaternion &a)
{
    // 乗算と代入
    *this = (*this * a);
    return *this;
}

// 正規化
void Quaternion::normalize()
{
    // 四元数の大きさ
    float mag = sqrt(w * w + x * x + y * y + z + z);

    // 0除算から保護するために無効な長さをチェック
    if (mag > 0.0f)
    {
        // 正規化する
        float oneOverMag = 1.0f / mag;
        w *= oneOverMag;
        x *= oneOverMag;
        y *= oneOverMag;
        z *= oneOverMag;
    }
    else
    {
        // ヒューストン問題発生
        assert(false);

        // リリースビルドでは何かに割り当てる

        identity();
    }
}

// 回転角thetaを返す
float Quaternion::getRotationAngle() const
{
    // 半分の角度を計算する
    // w = cos(theta / 2)であることに注意
    float thetaOver2 = safeAcos(w);
    return thetaOver2 * 2.0f;
}

// 回転軸を返す
Vector3 Quaternion::getRotationAxis() const
{
    // sin^2(theta / 2)を計算する
    // w = cos(theta / 2), sin^2(x) + cos^2(x) = 1
    float sinThetaOver2Sq = 1.0f - w * w;

    // 数値の不正確さのチェック
    if (sinThetaOver2Sq <= 0.0f)
    {
        // 恒等四元数、または不正確な数値
        // 任意の有効なベクトルを返すだけ
        return Vector3(1.0f, 0.0f, 0.0f);
    }

    // 1 / sin(theta / 2)を計算する
    float oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

    return Vector3(
        x * oneOverSinThetaOver2,
        y * oneOverSinThetaOver2,
        z * oneOverSinThetaOver2);
}

// 四元数の内積
extern float dotProduct(const Quaternion &a, const Quaternion &b)
{
    return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

// 球面線形補完
extern Quaternion slerp(const Quaternion &q0, const Quaternion &q1, float t)
{
    if (t <= 0.0f)
        return q0;

    if (t >= 1.0f)
        return q1;

    float cosOmega = dotProduct(q0, q1);

    // 負の内積の場合、-q1を用いる
    // 2つの四元数qと-qは同じ回転を表すが、異なる球面線形補完を生成する場合がある
    // 正確な角度を用いて回転するためにqか-qかを選択する
    float q1w = q1.w;
    float q1x = q1.x;
    float q1y = q1.y;
    float q1z = q1.z;
    if (cosOmega < 0.0f)
    {
        q1w = -q1w;
        q1x = -q1x;
        q1y = -q1y;
        q1z = -q1z;
        cosOmega = -cosOmega;
    }

    // 2つの四元数は単位四元数である必要があり、内積は<=1.0になるはずである
    assert(cosOmega < 1.1f);

    // 補間用フラクションを計算する
    // 四元数がほとんどまったく同じであるかチェックする
    float k0, k1;
    if (cosOmega > 0.999f)
    {
        // 非常に近い --- 単純に線形補完を用いる(0除算を防ぐため)
        k0 = 1.0f - t;
        k1 = t;
    }
    else
    {
        // 三角法の等式sin^2(omega) + cos^2(omega) = 1を用いて
        // sinを算出する
        float sinOmega = sqrt(1.0f - cosOmega * cosOmega);

        // このsinとcosから角度を計算する
        float omega = atan2(sinOmega, cosOmega);

        // 除算が1回で済むように、分母の逆数を計算する
        float oneOverSinOmega = 1.0f / sinOmega;

        // 補間パラメータを計算する
        k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
        k1 = sin(t * omega) * oneOverSinOmega;
    }

    // 補間する
    Quaternion result;
    result.x = k0 * q0.x + k1 * q1x;
    result.y = k0 * q0.y + k1 * q1y;
    result.z = k0 * q0.z + k1 * q1z;
    result.w = k0 * q0.w + k1 * q1w;
    return result;
}

// 四元数の共役
extern Quaternion conjugate(const Quaternion &q)
{
    Quaternion result;
    result.w = q.w;
    result.x = -q.x;
    result.y = -q.y;
    result.z = -q.z;
    return result;
}

// 四元数の累乗
extern Quaternion pow(const Quaternion &q, float exponent)
{
    // 単位四元数チェック(0除算チェック)
    if (fabs(q.w) > 0.999f)
    {
        return q;
    }

    // 半分の角度alpha(alpha = theta / 2)を取り出す
    float alpha = acos(q.w);

    // 新しいalpha値を計算する
    float newAlpha = alpha * exponent;

    // 新しいw値を計算する
    Quaternion result;
    result.w = cos(newAlpha);

    // 新しいxyz値を計算する
    float mult = sin(newAlpha) / sin(alpha);
    result.x = q.x * mult;
    result.y = q.y * mult;
    result.z = q.z * mult;

    return result;
}
