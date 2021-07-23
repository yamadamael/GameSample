#pragma once

class Vector3;
class EulerAngles;

class Quaternion
{
public:
    // 四元数の4つの値
    // 通常、これらを直接操作する必要はない
    // しかしながら、これらはpublicにしておく
    // 直接アクセスを禁止すると、いくつかの操作(ファイルI/Oなど)が
    // 不必要に複雑になるからである
    float w, x, y, z;

    // 恒等四元数にする
    void identity()
    {
        w = 1.0f;
        x = y = z = 0.0f;
    }

    // 四元数を特定の角度にセットアップする
    void setToRotateAboutX(float theta);
    void setToRotateAboutY(float theta);
    void setToRotateAboutZ(float theta);
    void setToRotateAboutAxis(const Vector3 &axis, float theta);

    // オブジェクト空間<->慣性空間の回転を実行するようにセットアップする
    void setToRotateObjectToInertial(const EulerAngles &orientation);
    void setToRotateInertialToObject(const EulerAngles &orientation);

    // 外積
    Quaternion operator*(const Quaternion &a) const;

    // 代入を伴う乗算:C++の慣習による
    Quaternion &operator*=(const Quaternion &a);

    // 正規化
    void normalize();

    // 回転角と軸を取り出し返す
    float getRotationAngle() const;
    Vector3 getRotationAxis() const;
};

// 恒等四元数用のグローバル定数
extern const Quaternion kQuaternionIdentity;

// 四元数の内積
extern float dotProduct(const Quaternion &a, const Quaternion &b);

// 球面線形補完
extern Quaternion slerp(const Quaternion &q0, const Quaternion &q1, float t);

// 四元数の共役
extern Quaternion conjugate(const Quaternion &q);

// 四元数の累乗
extern Quaternion pow(const Quaternion &q, float exponent);
