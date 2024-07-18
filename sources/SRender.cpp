#include "SRender.hpp"

#include <string>
#include "CTransform.hpp"
#include "GameSettings.hpp"
#include "raylib.h"

#include "Stats.hpp"
void SRender::Init() {
    printf("Render init\n");
}
void SRender::Update(Registry* registry){
    //printf("Render update\n");
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (const auto ID: registry->getSparseSet<CTransform>()->getIDS()) {
        auto& transform = registry->getComponent<CTransform>(ID);
        DrawRectangle(transform.position.x,transform.position.y,50,50,{255,0,0,255});
    }
    DrawStatistics();
    EndDrawing();

}
void SRender::Shutdown() {}
void SRender::DrawStatistics() {
    DrawText(("FPS: " + std::to_string(GetFPS())).c_str(),50,0,50,{255,0,0,255});
    DrawText(("DeltaTime: " + std::to_string(GetFrameTime())).c_str(),50,50,50,{255,0,0,255});
    DrawText(("Update: " + std::to_string(Stats::timerVector[Stats::StatType::UPDATE].count())).c_str(),50,100,50,{255,0,0,255});

}
SRender::SRender() {
    printf("Render system created\n");

}
SRender::~SRender() {
    printf("Render system destroyed\n");
}
