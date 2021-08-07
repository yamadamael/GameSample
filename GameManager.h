#pragma once

class SpriteManager;

class GameManager
{
protected:
    static GameManager *instance;
    GameManager();

public:
    SpriteManager *spriteManager;

    static void Create();
    static GameManager &GetInstance()
    {
        if (instance == nullptr)
        {
            Create();
        }

        return *instance;
    }
};
