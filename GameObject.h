#pragma once

#include <windows.h>
#include <vector>

class Vector3;
class EulerAngles;
class Quaternion;
class RotationMatrix;
class Matrix4x3;
class bitmap;

class GameObject
{
private:
    Vector3 *position;
    Quaternion *rotation;
    Vector3 *scale;
    Matrix4x3 *matrix;

public:
    const char *Name;
    GameObject *Parent;
    std::vector<GameObject> Children;

    bitmap *Bitmap;

    GameObject();
    GameObject(const char *name);

    void SetParent(GameObject *go);

    void SetPosition(Vector3 *position);
    void SetRotation(Quaternion *rotation);
    void SetScale(Vector3 *scale);
    // void SetMatrix(Matrix4x3 *matrix);

    Vector3 *GetPosition();
    Quaternion *GetRotation();
    Vector3 *GetScale();
    Matrix4x3 *GetMatrix();

    Vector3 *GetWorldPosition();
    Quaternion *GetWorldRotation();
    Vector3 *GetWorldScale();
    Matrix4x3 *GetWorldMatrix();

    void UpdateMatrix();

    void ReadBmp(const char *path);

    void Draw(const HDC &hdc);

    void Destroy();
};
