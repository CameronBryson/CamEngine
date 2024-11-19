#pragma once
#include "Collision.hpp"
#include "Commands.hpp"
#include "ISparseSet.hpp"
#include "SparseSet.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <functional>
#include <memory>
#include <numeric>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "CollisionEvents.hpp"
#include "EventHandler.hpp"
#include "Factory.hpp"

enum class KeyAction
{
    None,
    Start,
    Hold,
    End
};
struct HashPair {
    std::size_t operator()(const std::pair<unsigned short, unsigned short>& p) const {
        const auto hash1 = std::hash<unsigned short>{}(p.first);
        const auto hash2 = std::hash<unsigned short>{}(p.second);
        // Ensure the order does not change the hash
        return hash1 ^ hash2 ^ std::hash<unsigned short>{}(std::min(p.first, p.second)) ^ std::hash<unsigned short>{}(std::max(p.first, p.second));
    }
};
class Registry
{
public:

    Registry();
    ~Registry();

    unsigned short createEntity();
    void deleteEntity(unsigned short id);
    void processCommands();
    void onCollisionDetectedEvent(const Event<CollisionEvents>& event);
    void onCollisionNotDetectedEvent(const Event<CollisionEvents>& event);
    void onCollisionEnterEvent(const Event<CollisionEvents>& event);
    void onCollisionExitEvent(const Event<CollisionEvents>& event);
    void onInputPressEvent(const Event<InputEvents>& event);
    void onInputReleaseEvent(const Event<InputEvents>& event);

    void processCollisionResolutions();
    KeyAction getKeyAction(int key);
    void resetKeyStates();

public:
    template <typename T>
    void createSparseSet();

    template <typename T,typename... Args>
    void addComponent(unsigned short id, Args&&... componentArgs);


    template <typename T>
    T& getComponent(unsigned short id) const;

    template <typename T>
    [[nodiscard]] bool hasComponent(unsigned short id) const;

    template <typename T>
    void removeComponent(unsigned short id);

    template <typename... T>
    [[nodiscard]] std::vector<unsigned short> getEntityIDs() const;

    template <typename T>
    SparseSet<T>& getSparseSet() const;

    template <typename T>
    [[nodiscard]] bool hasSparseSet() const;

private:
    std::vector<unsigned short> mFreeIDs;
    std::vector<unsigned short> mEntities;
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> mSparseSets;
    std::queue<std::unique_ptr<ICommand>> mCommandQueue;
    std::unordered_map<std::pair<unsigned short, unsigned short>, std::unique_ptr<CollisionManifold>,HashPair> mCollisionMap;
    std::unordered_map<int, KeyAction> mKeyMap;
    //some kind of key storage
};

#include "Registry.tpp"
