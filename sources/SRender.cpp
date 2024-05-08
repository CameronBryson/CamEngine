#include "SRender.hpp"

#include "GameSettings.hpp"
#include "raylib.h"
void SRender::Init() {
    InitWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "Window");
    SetTargetFPS(Settings::MAX_FPS);
}
void SRender::Update(){

}
void SRender::Shutdown(){
    CloseWindow();
  
}