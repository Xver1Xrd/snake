#include "raylib.h"
#include "Game.h"

Game game;

void UpdateDrawFrame()
{
    game.HandleInput();
    game.Update();

    BeginDrawing();
    ClearBackground((Color){ 10, 10, 18, 255 });
    game.Draw();
    EndDrawing();
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(Game::SW, Game::SH, "TimeEcho Snake");
    SetTargetFPS(60);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();
    return 0;
}
