#include "SRender.hpp"

#include "CTransform.hpp"
#include "GameSettings.hpp"
#include "raylib.h"
void SRender::Init() {
    //InitWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "Window");
    //SetTargetFPS(Settings::MAX_FPS);
}
void SRender::Update(Registry& registry){
    //BeginDrawing();
    //learBackground(RAYWHITE);
    for (auto ID: registry.getSparseSet<CTransform>()->getIDS()) {
        auto& transform = registry.getComponent<CTransform>(ID);
        //DrawRectangle(transform.position.x,transform.position.y,50,50,{255,0,0,255});
    }
    //EndDrawing();

}
void SRender::Shutdown(){

}