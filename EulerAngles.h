#pragma once

class Quaternion;
class Matrix4x3;
class RotationMatrix;

class EulerAngles
{

public:
    float heading;
    float pitch;
    float bank;

    // デフォルトコンストラクタ
    EulerAngles() : heading(0), pitch(0), bank(0) {}

    // コンストラクタ(3値)
    EulerAngles(float h, float p, float b) : heading(h), pitch(p), bank(b) {}

    // 恒等化
    void identity() { heading = pitch = bank = 0.0f; }

    // 正準オイラー角の3つ組を決定する
    void canonize();

    // 四元数をオイラー角形式に変換する
    // 入力される四元数は、その名前が示すようにオブジェクト空間から慣性空間、
    // または、慣性空間からオブジェクト空間への回転を実行するものとする
    void fromObjectToInertialQuaternion(const Quaternion &q);
    void fromInertialToObjectQuaternion(const Quaternion &q);

    // 座標変換行列をオイラー角形式に変換する
    // 入力される行列は、その名前が示すようにオブジェクト空間から慣性空間、
    // または、慣性空間からオブジェクト空間への座標変換を実行するものとする
    // この行列は直交しているものとする
    void fromObjectToWorldMatrix(const Matrix4x3 &m);
    void fromWorldToObjectMatrix(const Matrix4x3 &m);

    // 回転行列をオイラー角形式に変換する
    void fromRotationMatrix(const RotationMatrix &m);
};

// 恒等オイラー角用のグローバルな定数
extern const EulerAngles kEulerAnglesIdentity;
