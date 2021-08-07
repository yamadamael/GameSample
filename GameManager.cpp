#include "GameManager.h"
#include "SpriteManager.h"

GameManager *GameManager::instance = nullptr;

GameManager::GameManager()
{
    spriteManager = new SpriteManager();
}

void GameManager::Create()
{
    if (!instance)
    {
        instance = new GameManager();
    }
}

// GameManager &GameManager::GetInstance()
// {
//     if (instance == NULL)
//     {
//         Create();
//     }
//     return *instance;
// }
