#include <assert.h>
#include <math.h>

#include "Vector3.h"
#include "EulerAngles.h"
#include "Quaternion.h"
#include "RotationMatrix.h"
#include "Matrix4x3.h"
#include "MathUtil.h"

// 恒等行列を設定する
void Matrix4x3::identity()
{
    m11 = 1.0f;
    m12 = 0.0f;
    m13 = 0.0f;
    m21 = 0.0f;
    m22 = 1.0f;
    m23 = 0.0f;
    m31 = 0.0f;
    m32 = 0.0f;
    m33 = 1.0f;
    tx = ty = tz = 0.0f;
}

// 4番目の行を0にする
void Matrix4x3::zeroTranslation()
{
    tx = ty = tz = 0.0f;
}

// 平行移動部分を設定する
void Matrix4x3::setTranslation(const Vector3 &d)
{
    tx = d.x;
    ty = d.y;
    tz = d.z;
}

// 平行移動部分を設定し、恒等行列を設定する
void Matrix4x3::setupTranslation(const Vector3 &d)
{
    m11 = 1.0f;
    m12 = 0.0f;
    m13 = 0.0f;
    m21 = 0.0f;
    m22 = 1.0f;
    m23 = 0.0f;
    m31 = 0.0f;
    m32 = 0.0f;
    m33 = 1.0f;
    tx = d.x;
    ty = d.y;
    tz = d.z;
}

// 親の基準座標系のローカルの基準座標系の位置と方向を設定し、
// ローカル->親への座標変換を実行する行列をセットアップする

// オブジェクト行列->ワールド行列の作成に用いる
//
void Matrix4x3::setupLocalToParent(const Vector3 &pos, const EulerAngles &orient)
{
    RotationMatrix orientMatrix;
    orientMatrix.setup(orient);
    setupLocalToParent(pos, orientMatrix);
}

void Matrix4x3::setupLocalToParent(const Vector3 &pos, const RotationMatrix &orient)
{
    // 回転行列(RotationMatrix)は通常、
    // 慣性空間->オブジェクト空間への行列であり親->ローカルである
    // ローカル->親への回転が欲しいためコピー中に転置する
    m11 = orient.m11;
    m12 = orient.m21;
    m13 = orient.m31;
    m21 = orient.m12;
    m22 = orient.m22;
    m23 = orient.m32;
    m31 = orient.m13;
    m32 = orient.m23;
    m33 = orient.m33;

    // 回転後に平行移動をするため直接コピーするだけ
    tx = pos.x;
    ty = pos.y;
    tz = pos.z;
}

// 親の基準座標系のローカルの基準座標系の位置と方向を設定し、
// 親->ローカルへの座標変換を実行する行列をセットアップする

// ワールド行列->オブジェクト行列の作成に用いる
// 座標変換を実行するには、通常最初にワールドから慣性空間へ座標変換し、
// その後、慣性空間からオブジェクト空間へ回転する
// しかし、4x3行列は常に最後に平行移動を行う
// このため、2つの行列TとRの作成を考え、M=TRという形で連結する
void Matrix4x3::setupParentToLocal(const Vector3 &pos, const EulerAngles &orient)
{
    RotationMatrix orientMatrix;
    orientMatrix.setup(orient);
    setupParentToLocal(pos, orientMatrix);
}

void Matrix4x3::setupParentToLocal(const Vector3 &pos, const RotationMatrix &orient)
{
    // 回転行列(RotationMatrix)から転置なしでそのままコピーできる
    m11 = orient.m11;
    m12 = orient.m12;
    m13 = orient.m13;
    m21 = orient.m21;
    m22 = orient.m22;
    m23 = orient.m23;
    m31 = orient.m31;
    m32 = orient.m32;
    m33 = orient.m33;

    // 通常、この位置分だけ逆に平行移動し、ワールド空間から慣性空間へ平行移動する
    // しかし、この回転が最初に起こるため平行移動部分を回転する必要がある
    // これは-posだけ平行移動する行列Tと回転行列Rを作成し、
    // 連結したTR行列を作成することと同じである
    tx = -(pos.x * m11 + pos.y * m21 + pos.z * m31);
    ty = -(pos.x * m12 + pos.y * m22 + pos.z * m32);
    tz = -(pos.x * m13 + pos.y * m23 + pos.z * m33);
}

// 基本軸の周りの回転を行う行列をセットアップする
// axis
//  1:x軸に関する回転
//  2:y軸に関する回転
//  3:z軸に関する回転
// theta
//  回転量(ラジアン)。左手のルールで正の回転を定義する
// 平行移動部分はリセットされる
void Matrix4x3::setupRotate(int axis, float theta)
{
    float s, c;
    sinCos(&s, &c, theta);

    switch (axis)
    {
    case 1:
        m11 = 1.0f;
        m12 = 0.0f;
        m13 = 0.0f;
        m21 = 0.0f;
        m22 = c;
        m23 = s;
        m31 = 0.0f;
        m32 = -s;
        m33 = c;
        break;
    case 2:
        m21 = c;
        m22 = 0.0f;
        m23 = -s;
        m21 = 0.0f;
        m22 = 1.0f;
        m23 = 0.0f;
        m31 = s;
        m32 = 0.0f;
        m33 = c;
        break;
    case 3:
        m11 = c;
        m12 = s;
        m13 = 0.0f;
        m21 = -s;
        m22 = c;
        m23 = 0.0f;
        m31 = 0.0f;
        m32 = 0.0f;
        m33 = 1.0f;
        break;
    default:
        assert(false);
        break;
    }

    tx = ty = tz = 0.0f;
}

// 任意の軸の周りの回転を行う行列をセットアップする
// 回転軸は原点を通らなければならない
// axis
//  回転軸。単位ベクトルでなければならない
// theta
//  回転量(ラジアン)。左手のルールで正の回転を定義する
// 平行移動部分はリセットされる
void Matrix4x3::setupRotate(const Vector3 &axis, float theta)
{
    // 軸を指定するのに単位ベクトルが渡されたかどうかをチェックする
    assert(fabs(axis * axis - 1.0f) < 0.01f);

    // 回転角のsinとcosを得る
    float s, c;
    sinCos(&s, &c, theta);

    // 1-cos(theta)といくつかの共通して用いる副次式を計算する
    float a = 1.0f - c;
    float ax = a * axis.x;
    float ay = a * axis.y;
    float az = a * axis.z;

    // 行列の要素を設定する
    // 共通に用いられる副次式がたくさんあるため、もう少し最適化の余地がある
    // それはコンパイラに任せるとする
    m11 = ax * axis.x + c;
    m12 = ax * axis.y + axis.z * s;
    m13 = ax * axis.z + axis.y * s;

    m21 = ay * axis.x - axis.z * s;
    m22 = ay * axis.y + c;
    m23 = ay * axis.z + axis.x * s;

    m31 = az * axis.x + axis.y * s;
    m32 = az * axis.y + axis.x * s;
    m33 = az * axis.z + c;

    // 平行移動はリセットする
    tx = ty = tz = 0.0f;
}

// 回転を行う行列をセットアップする
// 角変位は四元数形式で与え、回転を実行する行列をセットアップする
// 平行移動部分はリセットされる
void Matrix4x3::fromQuaternion(const Quaternion &q)
{
    // 共通して用いる副次式を最適化するための計算
    float ww = 2.0f * q.w;
    float xx = 2.0f * q.x;
    float yy = 2.0f * q.y;
    float zz = 2.0f * q.z;

    // 行列の要素を設定する
    // 共通に用いられる副次式がたくさんあるため、もう少し最適化の余地がある
    // それはコンパイラに任せるとする
    m11 = 1.0f - yy * q.y - zz * q.z;
    m12 = xx * q.y + ww * q.z;
    m13 = xx * q.z - ww * q.y;

    m21 = xx * q.y - ww * q.z;
    m22 = 1.0f - xx * q.x - zz * q.z;
    m23 = yy * q.z + ww * q.y;

    m31 = xx * q.z - ww * q.y;
    m32 = xx * q.y + ww * q.z;
    m33 = 1.0f - yy * q.y - zz * q.z;

    // 平行移動はリセットする
    tx = ty = tz = 0.0f;
}

// 各軸でスケーリングを実行する行列をセットアップする
// kだけ均等スケーリングする場合はVector3(k,k,k)を用いる
// 平行移動部分はリセットされる
void Matrix4x3::setupScale(const Vector3 &s)
{
    m11 = s.x;
    m12 = 0.0f;
    m13 = 0.0f;

    m21 = 0.0f;
    m22 = s.y;
    m23 = 0.0f;

    m31 = 0.0f;
    m32 = 0.0f;
    m33 = s.z;

    // 平行移動はリセットする
    tx = ty = tz = 0.0f;
}

// 任意の軸に沿ってスケーリングを実行する行列をセットアップする
// axis
//  回転軸。単位ベクトルでなければならない
// 平行移動部分はリセットされる
void Matrix4x3::setupScaleAlongAxis(const Vector3 &axis, float k)
{
    // 単位ベクトルチェック
    assert(fabs(axis * axis - 1.0f) < 0.01f);

    // k-1といくつかの共通して用いる副次式
    float a = k - 1.0f;
    float ax = a * axis.x;
    float ay = a * axis.y;
    float az = a * axis.z;

    m11 = ax * axis.x + 1.0f;
    m22 = ay * axis.y + 1.0f;
    m33 = az * axis.z + 1.0f;

    m12 = m21 = ax * axis.y;
    m23 = m32 = ay * axis.z;
    m31 = m13 = az * axis.x;

    // 平行移動はリセットする
    tx = ty = tz = 0.0f;
}

// せん断を実行する行列をセットアップする
// axis
//  1: y+=s*x, z+=t*x
//  2: x+=s*y, z+=t*y
//  3: x+=s*z, y+=t*z
// 平行移動部分はリセットされる
void Matrix4x3::setupShear(int axis, float s, float t)
{
    switch (axis)
    {
    case 1:
        m11 = 1.0f;
        m12 = s;
        m13 = t;
        m21 = 0.0f;
        m22 = 1.0f;
        m23 = 0.0f;
        m31 = 0.0f;
        m32 = 0.0f;
        m33 = 1.0f;
        break;
    case 2:
        m11 = 1.0f;
        m12 = 0.0f;
        m13 = 0.0f;
        m21 = s;
        m22 = 1.0f;
        m23 = t;
        m31 = 0.0f;
        m32 = 0.0f;
        m33 = 1.0f;
        break;
    case 3:
        m11 = 1.0f;
        m12 = 0.0f;
        m13 = 0.0f;
        m21 = 0.0f;
        m22 = 1.0f;
        m23 = 0.0f;
        m31 = s;
        m32 = t;
        m33 = 1.0f;
        break;
    default:
        assert(false);
        break;
    }

    // 平行移動はリセットする
    tx = ty = tz = 0.0f;
}

// 原点を通る平面上への投影を実行する行列をセットアップする
// この面は単位ベクトルnに垂直になる
void Matrix4x3::setupProject(const Vector3 &n)
{
    // 単位ベクトルチェック
    assert(fabs(n * n - 1.0f) < 0.01f);

    m11 = 1.0f - n.x * n.x;
    m22 = 1.0f - n.y * n.y;
    m33 = 1.0f - n.z * n.z;

    m12 = m21 = -n.x * n.y;
    m23 = m32 = -n.y * n.z;
    m31 = m13 = -n.z * n.x;

    // 平行移動はリセットする
    tx = ty = tz = 0.0f;
}

// 基本面に平行な面に関してのリフレクションを実行する行列をセットアップする
// axis
//  1: 面x=kに関するリフレクション
//  2: 面y=kに関するリフレクション
//  3: 面z=kに関するリフレクション
// 平行移動は適切に設定される
void Matrix4x3::setupReflect(int axis, float k)
{
    switch (axis)
    {
    case 1:
        m11 = -1.0f;
        m12 = 0.0f;
        m13 = 0.0f;
        m21 = 0.0f;
        m22 = 1.0f;
        m23 = 0.0f;
        m31 = 0.0f;
        m32 = 0.0f;
        m33 = 1.0f;

        tx = 2.0f * k;
        ty = 0.0f;
        tz = 0.0f;
        break;
    case 2:
        m11 = 1.0f;
        m12 = 0.0f;
        m13 = 0.0f;
        m21 = 0.0f;
        m22 = -1.0f;
        m23 = 0.0f;
        m31 = 0.0f;
        m32 = 0.0f;
        m33 = 1.0f;

        tx = 0.0f;
        ty = 2.0f * k;
        tz = 0.0f;
        break;
    case 3:
        m11 = 1.0f;
        m12 = 0.0f;
        m13 = 0.0f;
        m21 = 0.0f;
        m22 = 1.0f;
        m23 = 0.0f;
        m31 = 0.0f;
        m32 = 0.0f;
        m33 = -1.0f;

        tx = 0.0f;
        ty = 0.0f;
        tz = 2.0f * k;
        break;
    default:
        assert(false);
        break;
    }
}

// 原点を通る任意の平面に関するリフレクションを実行する行列をセットアップする
// この面は単位ベクトルnに垂直になる
// 平行移動部分はリセットされる
void Matrix4x3::setupReflect(const Vector3 &n)
{
    assert(fabs(n * n - 1.0f) < 0.01f);

    float ax = -2.0f * n.x;
    float ay = -2.0f * n.y;
    float az = -2.0f * n.z;

    m11 = 1.0f + ax * n.x;
    m22 = 1.0f + ay * n.y;
    m33 = 1.0f + az * n.z;

    m12 = m21 = ax * n.y;
    m13 = m31 = az * n.x;
    m23 = m32 = ay * n.z;

    // 平行移動はリセットする
    tx = ty = tz = 0.0f;
}

Vector3 *Matrix4x3::GetPosition() const
{
    return new Vector3(tx, ty, tz);
}

RotationMatrix *Matrix4x3::GetRotationMatrix(bool isUseScale) const
{
    auto rotationMatrix = new RotationMatrix();
    rotationMatrix->setup(*this, isUseScale);

    return rotationMatrix;
}

Vector3 *Matrix4x3::GetScale() const
{
    auto sx = vectorMag(Vector3(1, 0, 0));
    auto sy = vectorMag(Vector3(0, 1, 0));
    auto sz = vectorMag(Vector3(0, 0, 1));

    return new Vector3(sx, sy, sz);
}

// 点を座標変換する
Vector3 operator*(const Vector3 &p, const Matrix4x3 &m)
{
    return Vector3(
        p.x * m.m11 + p.y * m.m21 + p.z * m.m31 + m.tx,
        p.x * m.m12 + p.y * m.m22 + p.z * m.m32 + m.ty,
        p.x * m.m13 + p.y * m.m23 + p.z * m.m33 + m.tz);
}

Vector3 &operator*=(Vector3 &p, const Matrix4x3 &m)
{
    p = p * m;
    return p;
}

// 行列の連結
Matrix4x3 operator*(const Matrix4x3 &a, const Matrix4x3 &b)
{
    Matrix4x3 r;

    r.m11 = a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31;
    r.m12 = a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32;
    r.m13 = a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33;

    r.m21 = a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31;
    r.m22 = a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32;
    r.m23 = a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33;

    r.m31 = a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31;
    r.m32 = a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32;
    r.m33 = a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33;

    r.tx = a.tx * b.m11 + a.ty * b.m21 + a.tz * b.m31 + b.tx;
    r.ty = a.ty * b.m12 + a.ty * b.m22 + a.tz * b.m32 + b.ty;
    r.tz = a.tz * b.m13 + a.ty * b.m23 + a.tz * b.m33 + b.tz;

    return r;
}

Matrix4x3 &operator*=(Matrix4x3 &a, const Matrix4x3 &b)
{
    a = a * b;
    return a;
}

// 行列の3x3部分の行列式の計算
float determinant(const Matrix4x3 &m)
{
    return m.m11 * (m.m22 * m.m33 - m.m23 * m.m32) +
           m.m12 * (m.m23 * m.m31 - m.m21 * m.m33) +
           m.m13 * (m.m21 * m.m32 - m.m22 * m.m31);
}

// 逆行列を計算する
Matrix4x3 inverse(const Matrix4x3 &m)
{
    // 行列式
    float det = determinant(m);
    assert(fabs(det) > 0.000001f);

    float oneOverDet = 1.0f / det;

    // 逆行列を計算する
    Matrix4x3 r;
    r.m11 = (m.m22 * m.m33 - m.m23 * m.m32) * oneOverDet;
    r.m12 = (m.m13 * m.m32 - m.m12 * m.m33) * oneOverDet;
    r.m13 = (m.m12 * m.m23 - m.m13 * m.m22) * oneOverDet;

    r.m21 = (m.m23 * m.m31 - m.m21 * m.m33) * oneOverDet;
    r.m22 = (m.m11 * m.m33 - m.m13 * m.m31) * oneOverDet;
    r.m23 = (m.m13 * m.m21 - m.m11 * m.m23) * oneOverDet;

    r.m31 = (m.m21 * m.m32 - m.m22 * m.m31) * oneOverDet;
    r.m32 = (m.m12 * m.m31 - m.m11 * m.m32) * oneOverDet;
    r.m33 = (m.m11 * m.m22 - m.m12 * m.m21) * oneOverDet;

    r.tx = -(m.tx * r.m11 + m.ty * r.m21 + m.tz * r.m31);
    r.ty = -(m.ty * r.m12 + m.ty * r.m22 + m.tz * r.m32);
    r.tz = -(m.tz * r.m13 + m.ty * r.m23 + m.tz * r.m33);

    return r;
}

// 行列から平行移動部分を取り出す
Vector3 getTranslation(const Matrix4x3 &m)
{
    return Vector3(m.tx, m.ty, m.tz);
}

// 親->ローカル座標変換行列(ワールド行列->オブジェクト行列など)を与え
// オブジェクトの位置を取り出す
// この行列は剛体変換を表しているという前提がある
// スケーリング、スキュー、ミラーを含まない
Vector3 getPositionFromParentToLocalMatrix(const Matrix4x3 &m)
{
    // 3x3の転置と負の平行移動値を乗算する
    // 転置を用いるので行列は直行行列であることが前提
    // 非剛体変換では意味がない
    return Vector3(-(m.tx * m.m11 + m.ty * m.m12 + m.tz * m.m13),
                   -(m.tx * m.m21 + m.ty * m.m22 + m.tz * m.m23),
                   -(m.tx * m.m31 + m.ty * m.m32 + m.tz * m.m33));
}

// ローカル->親座標変換行列(オブジェクト行列->ワールド行列など)を与え
// オブジェクトの位置を取り出す
Vector3 getPositionFromLocalToParentMatrix(const Matrix4x3 &m)
{
    return Vector3(m.tx, m.ty, m.tz);
}
