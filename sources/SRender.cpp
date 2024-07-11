#include "SRender.hpp"

#include "CTransform.hpp"
#include "GameSettings.hpp"
#include "raylib.h"
void SRender::Init() {
}
void SRender::Update(Registry* registry){
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
    //DrawText(("Update: " + std::to_string(Settings::UpdateEnd-Settings::UpdateStart)).c_str(),50,100,50,{255,0,0,255});

}
