#include "SRender.hpp"

#include <string>

#include "Components.hpp"
#include "GameSettings.hpp"
#include "raylib.h"

#include "Stats.hpp"
void SRender::Init() {
    printf("Render init\n");
}
void SRender::Update(Registry& registry) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    auto& positions = registry.getSparseSet<CPosition>();
    auto IDS = registry.getEntityIDS<CPosition>();
    for (auto ID : IDS) {
        auto& position = positions.getItem(ID);
        DrawRectangle(position.x, position.y, 50, 50, {255, 0, 0, 255});
    }
    DrawStatistics();
    EndDrawing();
}
void SRender::Shutdown() {}
void SRender::DrawStatistics() {
    DrawText(("FPS: " + std::to_string(GetFPS())).c_str(),50,0,50,{255,0,0,255});
    DrawText(("DeltaTime: " + std::to_string(GetFrameTime())).c_str(),50,50,50,{255,0,0,255});
    DrawText(("Update: " + std::to_string(Stats::timerVector[Stats::StatType::UPDATE].count()) + " MS").c_str(),50,100,50,{255,0,0,255});
    DrawText(("Render: " + std::to_string(Stats::timerVector[Stats::StatType::RENDER].count()) + " MS").c_str(),50,150,50,{255,0,0,255});
    DrawText(("Benchmark: " + std::to_string(Stats::timerVector[Stats::StatType::BENCHMARK].count()) + " MS").c_str(),50,200,50,{255,0,0,255});

}
