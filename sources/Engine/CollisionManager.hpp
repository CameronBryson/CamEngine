#pragma once
#include <unordered_map>
#include <memory>
#include <utility>
#include "CollisionManifold.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"

struct HashPair
{
    std::size_t operator()(const std::pair<unsigned short, unsigned short>& p) const
    {
        const auto hash1 = std::hash<unsigned short>{}(p.first);
        const auto hash2 = std::hash<unsigned short>{}(p.second);
        // Ensure the order does not change the hash
        return hash1 ^ hash2 ^ std::hash<unsigned short>{}(std::min(p.first, p.second)) ^ std::hash<unsigned short>{}(std::max(p.first, p.second));
    }
};

class CollisionManager {
public:
    CollisionManager();
    ~CollisionManager();
    void handleCollisionDetectedEvent(const Event<CollisionEvents>& event);
    void handleCollisionNotDetectedEvent(const Event<CollisionEvents>& event);
    void processCollisionResolutions();
    void clearCollisions();

private:
    std::unordered_map<std::pair<unsigned short, unsigned short>, std::unique_ptr<CollisionManifold>, HashPair> mCollisionMap;
};
