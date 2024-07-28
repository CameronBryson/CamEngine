#pragma once
#include <Registry.hpp>
#include "raylib.h"

class SRender{
public:
    static void Init();
    static void Update(Registry &registry, Camera3D &camera);
    static void Shutdown();
private:
    static void DrawStatistics();
};

