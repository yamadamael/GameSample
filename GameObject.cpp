#include "GameObject.h"
#include "SpriteManager.h"
#include "bitmap.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4x3.h"
#include <windows.h>

GameObject::GameObject()
{
    Name = "";
    position = new Vector3();
    rotation = new Quaternion();
    rotation->identity();
    scale = new Vector3(1, 1, 1);
    matrix = new Matrix4x3();
    matrix->identity();

    Parent = NULL;

    Bitmap = new bitmap();
}

GameObject::GameObject(const char *name)
{
    Name = name;
    position = new Vector3();
    rotation = new Quaternion();
    rotation->identity();
    scale = new Vector3(1, 1, 1);
    matrix = new Matrix4x3();
    matrix->identity();

    Parent = NULL;

    Bitmap = new bitmap();
}

void GameObject::SetParent(GameObject *parent)
{
    Parent = parent;
    parent->Children.push_back(*this);
}

void GameObject::SetPosition(Vector3 *position)
{
    *this->position = *position;
    UpdateMatrix();
}

void GameObject::SetRotation(Quaternion *rotation)
{
    *this->rotation = *rotation;
    UpdateMatrix();
}

void GameObject::SetScale(Vector3 *scale)
{
    *this->scale = *scale;
    UpdateMatrix();
}

// void GameObject::SetMatrix(Matrix4x3 *matrix)
// {
//     *this->matrix = *matrix;
//     // position, rotation, scaleの逆算が必要
// }

Vector3 *GameObject::GetPosition()
{
    return position;
}

Quaternion *GameObject::GetRotation()
{
    return rotation;
}

Vector3 *GameObject::GetScale()
{
    return scale;
}

Matrix4x3 *GameObject::GetMatrix()
{
    return matrix;
}

Vector3 *GameObject::GetWorldPosition()
{
    if (Parent == NULL)
    {
        return position;
    }

    auto p = *position * *Parent->GetWorldMatrix();

    return &((*position) * (*(Parent->GetWorldMatrix())));
}

Quaternion *GameObject::GetWorldRotation()
{
    if (Parent == NULL)
    {
        return rotation;
    }

    auto p = *rotation * *Parent->GetWorldRotation();

    return &p;
}

Vector3 *GameObject::GetWorldScale()
{
    if (Parent == NULL)
    {
        return scale;
    }

    auto parentMatrix = Parent->GetWorldMatrix();
    auto sx = vectorMag(Vector3(1, 0, 0) * *parentMatrix);
    auto sy = vectorMag(Vector3(0, 1, 0) * *parentMatrix);
    auto sz = vectorMag(Vector3(0, 0, 1) * *parentMatrix);

    return new Vector3(sx, sy, sz);
}

Matrix4x3 *GameObject::GetWorldMatrix()
{
    if (Parent == NULL)
    {
        return matrix;
    }

    auto parentMatrix = Parent->GetWorldMatrix();
    return &((*parentMatrix) * (*matrix));
}

void GameObject::UpdateMatrix()
{
    matrix->fromQuaternion(*rotation);

    Matrix4x3 scaleMatrix;
    scaleMatrix.setupScale(*scale);
    *matrix = *matrix * scaleMatrix;

    matrix->setTranslation(*position);
}

void GameObject::ReadBmp(const char *path)
{
    Bitmap->Read_Bmp(path);
}

void GameObject::Draw(const HDC &hdc)
{
    if (Bitmap != NULL)
    {
        auto position = GetWorldPosition();
        Bitmap->Draw_Bmp(hdc, position->x, position->y);
        Bitmap->Draw_Bmp(hdc, *matrix);
    }

    for (auto itr = Children.begin(); itr != Children.end(); ++itr)
    {
        itr->Draw(hdc);
    }
}

void GameObject::Destroy()
{
    if (Bitmap != NULL)
    {
        Bitmap->Free_Image();
    }
}
