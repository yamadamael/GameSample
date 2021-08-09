#pragma once

class Vector3;
class EulerAngles;
class Quaternion;
class Matrix4x3;

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
    void setup(const Matrix4x3 &matrix, bool isUseScale);

    // 四元数から行列をセットアップする
    // この四元数は、指定された座標変換の向きで回転を実行するものとする
    void fromInertialToObjectQuaternion(const Quaternion &q);
    void fromObjectToInertialQuaternion(const Quaternion &q);

    // 回転を実行する
    Vector3 inertialToObject(const Vector3 &v);
    Vector3 ObjectToInertial(const Vector3 &v);
};
