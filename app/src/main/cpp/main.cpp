#define RAYGUI_IMPLEMENTATION

#include "game.h"

bool gameRunning;
Image icon;

void PrepairWindow()
{
    gameRunning = true;
    icon = LoadImage("resources/icon_64.png");
    

    if (game.GetSetting("Windowed")) {
        InitWindow(1280, 720, "The Empty Space");
        SetWindowState(FLAG_WINDOW_RESIZABLE);
        SetWindowState(FLAG_WINDOW_MAXIMIZED);
    }
    else {
        InitWindow(0, 0, "The Empty Space");
        SetWindowState(FLAG_FULLSCREEN_MODE);
    }
    SetExitKey(KEY_NULL);

    SetTargetFPS(game.GetSetting("Max fps") == 121 ? INT16_MAX : game.GetSetting("Max fps"));

    GuiLoadStyle("resources/style_cyber.rgs");

    GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);          // Толщина границы
    GuiSetStyle(DEFAULT, TEXT_PADDING, 10);         // Отступы текста
    GuiSetStyle(DEFAULT, TEXT_SIZE, 1.0 / 32 * GetScreenWidth());

    SetWindowIcon(icon);
    game.InitializeInterface();
}

int main()
{
    PrepairWindow();

    while (!WindowShouldClose() && gameRunning)
    {
        float dt = GetFrameTime();
        game.Step(dt);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        

        ClearBackground(BLACK);

        game.Render();

        DrawText(TextFormat("FPS: %d", GetFPS()), GetScreenWidth() - MeasureText(TextFormat("FPS: %d", GetFPS()), 30) - 30,
            30, 30, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

        EndDrawing();
    }

    UnloadImage(icon);
    CloseWindow();
    return 0;
}