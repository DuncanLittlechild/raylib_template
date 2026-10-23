#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"


int main ()
{
    InitWindow(1280, 720, "Test");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        ImGui::Begin("tab");
        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }
    rlImGuiEnd();
    CloseWindow();
    return 0;
}