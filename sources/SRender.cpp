#include "SRender.hpp"

#include <string>

#include "Components.hpp"
#include "GameSettings.hpp"
#include "raylib.h"

#include "Stats.hpp"

void SRender::Init() {
    printf("Render init\n");
}

void SRender::Update(Registry& registry, Camera3D& camera) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);

    // Draw CTransform (Cubes)
    auto& positions = registry.getSparseSet<CTransform>();
    auto IDS = registry.getEntityIDS<CTransform>();

    // Draw CSphere
    auto& spheres = registry.getSparseSet<CSphere>();
    IDS = registry.getEntityIDS<CSphere>();
    for (auto ID : IDS) {
        auto& sphere = spheres.getItem(ID);
        auto& position = positions.getItem(ID); // Assuming CTransform is also present
        DrawSphere({position.position.x, position.position.y, position.position.z}, sphere.radius, BLUE);
        DrawSphereWires({position.position.x, position.position.y, position.position.z}, sphere.radius, 16, 16, DARKBLUE);
    }

    // Draw CAABB
    auto& aabbs = registry.getSparseSet<CAABB>();
    IDS = registry.getEntityIDS<CAABB>();
    for (auto ID : IDS) {
        auto& aabb = aabbs.getItem(ID);
        auto& position = positions.getItem(ID); // Assuming CTransform is also present
        Vec3 min = position.position - aabb.extents * 0.5f;
        Vec3 max = position.position + aabb.extents * 0.5f;
        DrawCube({position.position.x, position.position.y, position.position.z}, aabb.extents.x, aabb.extents.y, aabb.extents.z, GREEN);
        DrawCubeWires({position.position.x, position.position.y, position.position.z}, aabb.extents.x, aabb.extents.y, aabb.extents.z, DARKGREEN);
    }

    DrawGrid(10, 1.0f);
    EndMode3D();
    DrawStatistics();
    EndDrawing();
}

void SRender::Shutdown() {}

void SRender::DrawStatistics() {
    DrawText(("FPS: " + std::to_string(GetFPS())).c_str(), 50, 0, 50, {255, 0, 0, 255});
    DrawText(("DeltaTime: " + std::to_string(GetFrameTime())).c_str(), 50, 50, 50, {255, 0, 0, 255});
    DrawText(("Update: " + std::to_string(Stats::timerVector[Stats::StatType::UPDATE].count()) + " MS").c_str(), 50, 100, 50, {255, 0, 0, 255});
    DrawText(("Render: " + std::to_string(Stats::timerVector[Stats::StatType::RENDER].count()) + " MS").c_str(), 50, 150, 50, {255, 0, 0, 255});
    DrawText(("Benchmark: " + std::to_string(Stats::timerVector[Stats::StatType::BENCHMARK].count()) + " MS").c_str(), 50, 200, 50, {255, 0, 0, 255});
}