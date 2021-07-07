#pragma once

class Vector3;
class EulerAngles;
class Quaternion;

class RotationMatrix
{
public:
    float m11, m12, m13;
    float m21, m22, m23;
    float m31, m32, m33;

    // 恒等行列
    void identity();

    // 特定の向きを持つ行列をセットアップする
    void setup(const EulerAngles &orientation);

    // 四元数から行列をセットアップする
    // この四元数は、指定された座標変換の向きで回転を実行するものとする
    void fromIntertialToObjectQuaternion(const Quaternion &q);
    void fromObjectToIntertialQuaternion(const Quaternion &q);

    // 回転を実行する
    Vector3 intertialToObject(const Vector3 &v);
    Vector3 ObjectToIntertial(const Vector3 &v);
};
