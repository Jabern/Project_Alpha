#pragma once


#include <raylib.h>
#include "rlgl.h"
#include "raymath.h" 

#include <vector>
#include <string>
#include <iostream>

#include "Map.h"
#include "Skybox.h"
#include "Player.h"

#include "Light.h"

struct MenuItem
{
    std::string text;
    Color color;
};

enum class MenuType
{
    MainMenu,
    OptionsMenu
};

class Game
{
public:
    Game(int screenWidth, int screenHeight);
    ~Game();

    void Run();
    void Initialize();
    void Shutdown();

private:
    int screenWidth;
    int screenHeight;

    PhysicsManager physicsManager;

    const char* title;
    Camera3D camera = { 0 };

    Vector2 mousePosition;
    float cameraDistance = 10.0f; 
    float cameraAngleX = 45.0f; 
    float cameraAngleY = 0.0f; 

    float sensitivity = 1.0f;
    float maxAngleX = 10.0f;
    Vector3 cameraPosition;
    float mouseY;
    float mouseX;
    float cameraRotationX;
    float cameraRotationY;

    Light light;
    Shader shader;

    Map map;
    bool gameInitialized = false;
    Player player;

    std::vector<MenuItem> menuOptions;
    int selectedOption;

    MenuType currentMenuType;

    enum class GameState
    {
        Menu,
        Play,
        Shutdown,
        NONE
    };

    GameState gameState;
    Skybox SkyBox;

    void Update();
    void Draw();

    void DrawMenu();
    void Draw3DView();

    void SetMenuOptions(MenuType menuType);

     void WorldInitialize();

};