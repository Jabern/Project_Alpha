#include "Game.h"

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 800;

    Game game(screenWidth, screenHeight);
    game.Initialize();
    while (!WindowShouldClose())
    {
        game.Run();
    }
    return 0;
}
