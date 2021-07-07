#include <math.h>

#include "EulerAngles.h"
#include "Quaternion.h"
#include "MathUtil.h"
#include "Matrix4x3.h"
#include "RotationMatrix.h"

// 恒等オイラー角のグローバルな定数
// このオブジェクトが(他のオブジェクトに対して)いつ作られるのか正確にわからない場合がある
// このため、このオブジェクトは、初期化される前に参照可能である
// しかし、ほとんどの実装では、他のオブジェクトが作成される前のプログラムの
// スタートアップ時に何らかの方法で0に初期化される
const EulerAngles kEulerAnglesIdentity(0.0f, 0.0f, 0.0f);

// オイラー角の3つの組を正準値に設定する
// これは、このオイラー角の3D空間での方向の表現としての意味は変更しないが、
// この角度が角速度などの他の目的用であれば、この操作は有効でなくなる
void EulerAngles::canonize()
{
    // ピッチを-Pi~Piにラップ
    pitch = wrapPi(pitch);

    // 正準範囲-Pi/2~Pi/2の外側で行列pitchの裏側をチェック
    if (pitch < -kPiOver2)
    {
        pitch = -kPi - pitch;
        heading += kPi;
        bank += kPi;
    }
    else if (pitch > kPiOver2)
    {
        pitch = kPi - pitch;
        heading += kPi;
        bank += kPi;
    }

    // ジンバルロックチェック
    if (fabs(pitch) > kPiOver2 - 1e-4)
    {
        // ジンバルロック内
        // 垂直軸に関するすべての回転をhヘディングに割り当てる
        heading += bank;
        bank = 0;
    }
    else
    {
        // ジンバルロック外
        // バンクを-Pi~Piにラップ
        bank = wrapPi(bank);
    }

    // ヘディングを-Pi~Piにラップ
    heading = wrapPi(heading);
}

// オイラー角をセットアップする
// オブジェクト空間->慣性空間への回転四元数が与えられる
void EulerAngles::fromObjectToIntertialQuaternion(const Quaternion &q)
{
    // sin(pitch)を取り出す
    float sp = -2.0f * (q.y * q.z - q.w * q.x);

    // ジンバルロックチェック
    if (fabs(sp) > 0.999f)
    {
        heading = atan2(-q.x * q.z + q.w * q.y, 0.5f - q.y * q.y - q.z * q.z);
        pitch = kPiOver2 * sp;
        // ヘディングを計算し、バンクを0にする
        bank = 0.0f;
    }
    else
    {
        heading = atan2(q.x * q.z + q.w * q.y, 0.5f - q.x * q.x - q.y * q.y);
        // sin(pitch)<1 -> cos(pitch) != 0 のためsafeAcosは使わなくてよい
        pitch = asin(sp);
        bank = atan2(q.x * q.y + q.w * q.z, 0.5f - q.x * q.x - q.z * q.z);
    }
}

// オイラー角をセットアップする
// 慣性空間->オブジェクト空間への回転四元数が与えられる
void EulerAngles::fromIntertialToObjectQuaternion(const Quaternion &q)
{
    // sin(pitch)を取り出す
    float sp = -2.0f * (q.y * q.z - q.w * q.x);

    // ジンバルロックチェック
    if (fabs(sp) > 0.999f)
    {
        // ヘディングを計算し、バンクを0にする
        heading = atan2(-q.x * q.z - q.w * q.y, 0.5f - q.y * q.y - q.z * q.z);
        pitch = kPiOver2 * sp;
        bank = 0.0f;
    }
    else
    {
        heading = atan2(q.x * q.z - q.w * q.y, 0.5f - q.x * q.x - q.y * q.y);
        // sin(pitch)<1 -> cos(pitch) != 0 のためsafeAcosは使わなくてよい
        pitch = asin(sp);
        bank = atan2(q.x * q.y - q.w * q.z, 0.5f - q.x * q.x - q.z * q.z);
    }
}

// オイラー角をセットアップする
// オブジェクト空間->ワールド空間への座標変換行列が与えられる
void EulerAngles::fromObjectToWorldMatrix(const Matrix4x3 &m)
{
    // m32からsin(pitch)を取り出す
    float sp = -m.m32;

    // TODO p204 9.9999は誤記?
    // ジンバルロックチェック
    if (fabs(sp) > 0.999f)
    {
        heading = atan2(-m.m23, m.m11);
        pitch = kPiOver2 * sp;
        bank = 0.0f;
    }
    else
    {
        heading = atan2(m.m31, m.m33);
        pitch = asin(sp);
        bank = atan2(m.m12, m.m22);
    }
}

// オイラー角をセットアップする
// ワールド空間->オブジェクト空間への座標変換行列が与えられる
void EulerAngles::fromWorldToObjectMatrix(const Matrix4x3 &m)
{
    // m23からsin(pitch)を取り出す
    float sp = -m.m23;

    // TODO p205 9.9999は誤記?
    // ジンバルロックチェック
    if (fabs(sp) > 0.999f)
    {
        heading = atan2(-m.m31, m.m11);
        pitch = kPiOver2 * sp;
        bank = 0.0f;
    }
    else
    {
        heading = atan2(m.m13, m.m33);
        pitch = asin(sp);
        bank = atan2(m.m21, m.m22);
    }
}

// オイラー角をセットアップする
// 回転行列が与えられる
void EulerAngles::fromRotationMatrix(const RotationMatrix &m)
{
    // m23からsin(pitch)を取り出す
    float sp = -m.m23;

    // TODO p206 9.9999は誤記?
    // ジンバルロックチェック
    if (fabs(sp) > 0.999f)
    {
        heading = atan2(-m.m31, m.m11);
        pitch = kPiOver2 * sp;
        bank = 0.0f;
    }
    else
    {
        heading = atan2(m.m13, m.m33);
        pitch = asin(sp);
        bank = atan2(m.m21, m.m22);
    }
}
