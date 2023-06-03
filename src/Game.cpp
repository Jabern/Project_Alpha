#include "Game.h"


Game::Game(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), title("Project-Alpha"), gameState(GameState::Menu), currentMenuType(MenuType::MainMenu)
{
}

Game::~Game()
{
    if (gameState == GameState::Shutdown)
    {
        Shutdown();
    }
}

void Game::Run()
{
    if (gameState == GameState::Shutdown)
    {
        Shutdown();
    }
    else
    {
        SetExitKey(0);
        Update();
        Draw();
    }
}

void Game::Initialize()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(60);
    selectedOption = 0;

    SetMenuOptions(currentMenuType);

    camera.position = { 5.0f, 5.0f, 5.0f };    // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    shader = LoadShader("lighting.vs", "lighting.fs");

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    light.CreateLight(LightType::Directional, { -1.0f, 2.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, WHITE, shader);

    mousePosition = GetMouseDelta();

    cameraRotationX = 0.0f;
    cameraRotationY = 0.0f;
    DisableCursor();
}

void Game::WorldInitialize()
{
    physicsManager.Initialize(); // Init PhysX before everything 

    map.GenerateMap(&physicsManager);
    map.SetShader(shader);
    SkyBox.Load("models/animestyled_hdr.hdr", "models/skybox.png", true);

    player.SetPhysicsManager(&physicsManager);
    player.SetMapShape(map.GetCollisionShape());
    player.Load();

    player.SetShader(shader);
    
    physicsManager.SetPlayer(player.GetActor());
    gameInitialized = true;
}

void Game::Update()
{
    switch (gameState)
    {
    case GameState::Menu:
        if (IsKeyPressed(KEY_ENTER))
        {
            switch (currentMenuType)
            {
            case MenuType::MainMenu:
                if (selectedOption == 0)
                {
                    if (!gameInitialized)
                    {
                        WorldInitialize();
                    }
                    gameState = GameState::Play;
                }
                else if (selectedOption == 1)
                {
                    currentMenuType = MenuType::OptionsMenu;
                    SetMenuOptions(currentMenuType);
                }
                else if (selectedOption == 2)
                {
                    gameState = GameState::Shutdown;
                    break;
                }
                break;
            case MenuType::OptionsMenu:
                if (selectedOption == 0)
                {
                    // TODO: Implement options logic
                }
                else if (selectedOption == 1)
                {
                    currentMenuType = MenuType::MainMenu;
                    SetMenuOptions(currentMenuType);
                }
                break;
            default:
                break;
            }
        }
        break;
    case GameState::Play:
        if (IsKeyPressed(KEY_ESCAPE))
            gameState = GameState::Menu;

        // Camera distance control
        cameraDistance -= GetMouseWheelMove() * 0.1f;

        // Cap the camera distance
        if (cameraDistance < 1.0f)
            cameraDistance = 1.0f;
        else if (cameraDistance > 10.0f)
            cameraDistance = 10.0f;

        // Camera rotation
        cameraRotationX += GetMouseDelta().y * 0.01f;
        cameraRotationY -= GetMouseDelta().x * 0.01f;

        // Cap the camera rotation
        if (cameraRotationX > 1.5f)
            cameraRotationX = 1.5f;
        else if (cameraRotationX < -1.5f)
            cameraRotationX = -1.5f;

        // Calculate camera offset behind the player
        Vector3 cameraOffset;
        cameraOffset.x = -sinf(cameraRotationY);
        cameraOffset.y = -sinf(cameraRotationX);
        cameraOffset.z = -cosf(cameraRotationY);

        // Update camera position based on player position and rotation
        camera.position = Vector3Add(player.GetPosition(), Vector3Scale(cameraOffset, cameraDistance));
        camera.target = player.GetPosition();

        // Update player rotation to match camera rotation
        player.SetRotation({ cameraRotationX, cameraRotationY, 0.0f });
        
        player.Update();

        break;
    default:
        break;
    }

    if (gameState == GameState::Menu)
    {
        if (IsKeyPressed(KEY_UP))
            selectedOption--;
        else if (IsKeyPressed(KEY_DOWN))
            selectedOption++;

        if (selectedOption < 0)
            selectedOption = static_cast<int>(menuOptions.size()) - 1;
        else if (selectedOption >= static_cast<int>(menuOptions.size()))
            selectedOption = 0;
    }
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (gameState)
    {
    case GameState::Menu:
        DrawMenu();
        break;
    case GameState::Play:
        Draw3DView();
        break;
    default:
        break;
    }

    EndDrawing();
}

void Game::Shutdown()
{
    gameState = GameState::NONE;
    player.Unload();
    map.Unload();
    SkyBox.Unload();
    physicsManager.Cleanup();
    CloseWindow();
}

void Game::SetMenuOptions(MenuType menuType)
{
    menuOptions.clear();

    switch (menuType)
    {
    case MenuType::MainMenu:
        menuOptions.push_back({ "Play", MAROON });
        menuOptions.push_back({ "Options", MAROON });
        menuOptions.push_back({ "Quit", MAROON });
        break;
    case MenuType::OptionsMenu:
        menuOptions.push_back({ "Apply Options", GREEN });
        menuOptions.push_back({ "Back", MAROON });
        break;
    default:
        break;
    }
}

void Game::DrawMenu()
{
    DrawText(title, screenWidth / 2 - MeasureText(title, 30) / 2, 80, 30, DARKGRAY);
    // std::cout << "Selected Option : " << selectedOption << std::endl; // ugly debug to remove
    for (size_t i = 0; i < menuOptions.size(); i++)
    {
        if (static_cast<int>(i) == selectedOption)
            DrawText(menuOptions[i].text.c_str(), screenWidth / 2 - MeasureText(menuOptions[i].text.c_str(), 20) / 2, 180 + static_cast<int>(i) * 40, 20, menuOptions[i].color);
        else
            DrawText(menuOptions[i].text.c_str(), screenWidth / 2 - MeasureText(menuOptions[i].text.c_str(), 20) / 2, 180 + static_cast<int>(i) * 40, 20, DARKGRAY);
    }
}

void Game::Draw3DView()
{
    //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    DrawText("3D View", 20, 20, 30, BLACK);
    DrawText("Press Esc to Return to Menu", 20, 60, 20, DARKGRAY);

    // TODO : Implement 3D Logic and Entity system , fuck me

    ClearBackground(RAYWHITE);
    
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], Vector3ToFloat(camera.position), SHADER_UNIFORM_VEC3);

    BeginMode3D(camera);

    light.DrawLightSphere();
    light.UpdateValues();
    light.TEST_Input();

    physicsManager.Update();
    map.Draw();
    SkyBox.Draw();
    player.Draw();

    EndMode3D();

    DrawFPS(10, 10);
}