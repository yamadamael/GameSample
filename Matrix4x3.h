#pragma once

class Vector3;
class EulerAngles;
class Quaternion;
class RotationMatrix;

class Matrix4x3
{
public:
    // 行列の値
    // 基本的には上側の3x3部分は線形変換を含み、最後の行は平行移動部分である
    float m11, m12, m13;
    float m21, m22, m23;
    float m31, m32, m33;
    float tx, ty, tz;

    // 恒等行列を設定する
    void identity();

    // 行列の平行移動部分を直接アクセスする
    void zeroTranslation();
    void setTranslation(const Vector3 &d);
    void setupTranslation(const Vector3 &d);

    // 親空間<->ローカル空間で特定の座標変換を行う行列をセットアップする
    // ローカル空間は、親空間内で指定された位置と方向にあるとする
    // この方向は、オイラー角か回転行列のいずれかを用いて指定される場合がある
    void setupLocalToParent(const Vector3 &pos, const EulerAngles &orient);
    void setupLocalToParent(const Vector3 &pos, const RotationMatrix &orient);
    void setupParentToLocal(const Vector3 &pos, const EulerAngles &orient);
    void setupParentToLocal(const Vector3 &pos, const RotationMatrix &orient);

    // 基本軸の周りの回転を行う行列をセットアップする
    void setupRotate(int axis, float theta);

    // 任意の軸の周りの回転を行う行列をセットアップする
    void setupRotate(const Vector3 &axis, float theta);

    // 回転を行う行列をセットアップする
    // 角変位は四元数形式とする
    void fromQuaternion(const Quaternion &q);

    // 各軸でスケーリングを実行する行列をセットアップする
    void setupScale(const Vector3 &s);

    // 任意の軸に沿ってスケーリングを実行する行列をセットアップする
    void setupScaleAlongAxis(const Vector3 &axis, float k);

    // せん断を実行する行列をセットアップする
    void setupShear(int axis, float s, float t);

    // 原点を通る平面上への投影を実行する行列をセットアップする
    void setupProject(const Vector3 &n);

    // 基本面に平行な面に関してのリフレクションを実行する行列をセットアップする
    void setupReflect(int axis, float k = 0.0f);

    // 原点を通る任意の平面に関するリフレクションを実行する行列をセットアップする
    void setupReflect(const Vector3 &n);

    Vector3 *GetPosition() const;
    RotationMatrix *GetRotationMatrix(bool isUseScale) const;
    Vector3 *GetScale() const;
};

// 演算子*は点の座標変換に使われ、また行列の連結にも使われる
// 左から右への乗算の順番は、座標変換の順番と同じである
Vector3 operator*(const Vector3 &p, const Matrix4x3 &m);
Matrix4x3 operator*(const Matrix4x3 &a, const Matrix4x3 &b);

// 演算子*=はC++の標準に準拠する
Vector3 &operator*=(Vector3 &p, const Matrix4x3 &m);
Matrix4x3 &operator*=(const Matrix4x3 &a, const Matrix4x3 &b);

// 行列の3*3の部分の行列式を計算する
float determinant(const Matrix4x3 &m);

// 逆行列を計算する
Matrix4x3 inverse(const Matrix4x3 &m);

// 行列から平行移動部分を取り出す
Vector3 getTranslation(const Matrix4x3 &m);

// 親->ローカル、またはローカル->親への行列から位置と方向を取り出す
Vector3 getPositionFromParentToLocalMatrix(const Matrix4x3 &m);
Vector3 getPositionFromLocalToParentMatrix(const Matrix4x3 &m);
