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
#include "Systems/SHealth.hpp"
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

    Registry()
    {
        printf("Registry created\n");
        mEntities.reserve(settings::max_entities);
        mFreeIDs.resize(settings::max_entities - 1);
        std::iota(mFreeIDs.begin(), mFreeIDs.end(), 1);
        for (int i = 0; i < 512; ++i)
        {
            mKeyMap[i] = KeyAction::None;
        }
        EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Detected, std::bind(&Registry::onCollisionDetectedEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::NotDetected, std::bind(&Registry::onCollisionNotDetectedEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Enter, std::bind(&Registry::onCollisionEnterEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Exit, std::bind(&Registry::onCollisionExitEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->inputDispatcher.AddListener(InputEvents::KeyPress, std::bind(&Registry::onInputPressEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->inputDispatcher.AddListener(InputEvents::KeyRelease, std::bind(&Registry::onInputReleaseEvent, this, std::placeholders::_1));

    };

    ~Registry()
    {
        processCommands();
        //remove all event bindings
        printf("Registry destroyed\n");
    }

public:
    unsigned short createEntity()
    {
        assert(!mFreeIDs.empty() && "No more entities available.");
        const unsigned short id = mFreeIDs.back();
        mFreeIDs.pop_back();
        mEntities.emplace_back(id);
        return id;
    };

    void deleteEntity(unsigned short id)
    {
        mCommandQueue.push(std::make_unique<DeleteEntityCommand>(mSparseSets, mFreeIDs, mEntities, id));
    };

    void processCommands()
    {
        while (!mCommandQueue.empty())
        {
            const auto& command = mCommandQueue.front();
            command->execute();
            mCommandQueue.pop();
        }
    }
    void onCollisionDetectedEvent(const Event<CollisionEvents>& event)
    {
        auto new_event = event.ToType<CollisionDetectedEvent>();
        auto pair = std::make_pair(new_event.id1, new_event.id2);
        if (!mCollisionMap.contains(pair))
        {
            //printf("Collision Enter\n");
            EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionEnterEvent(new_event.id1, new_event.id2));
        }
        else
        {
            //printf("Collision Stay\n");
            EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionStayEvent(new_event.id1, new_event.id2));
        }
        mCollisionMap[pair] = std::make_unique<CollisionManifold>(new_event.manifold);
    }
    void onCollisionNotDetectedEvent(const Event<CollisionEvents> &event){
        auto new_event = event.ToType<CollisionNotDetectedEvent>();
        auto pair = std::make_pair(new_event.id1, new_event.id2);
        if(mCollisionMap.contains(pair))
        {
            //printf("Collision Exit\n");
            EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionExitEvent(new_event.id1, new_event.id2));
            mCollisionMap.erase(pair);
        }
    }
    void onCollisionEnterEvent(const Event<CollisionEvents> &event){
        auto new_event = event.ToType<CollisionEnterEvent>();
        bool has_health1 = hasComponent<CHealth>(new_event.id1);
        bool has_health2 = hasComponent<CHealth>(new_event.id2);
        bool has_damage1 = hasComponent<CDamage>(new_event.id1);
        bool has_damage2 = hasComponent<CDamage>(new_event.id2);

        if (has_health1 && has_damage2)
        {
            auto& health = getComponent<CHealth>(new_event.id1);
            auto& damage = getComponent<CDamage>(new_event.id2);
            health.health-= damage.damage;
        }

        if (has_health2 && has_damage1)
        {
            auto& health = getComponent<CHealth>(new_event.id2);
            auto& damage = getComponent<CDamage>(new_event.id1);
            health.health-= damage.damage;
        }
    }
    void onCollisionExitEvent(const Event<CollisionEvents> &event){
        auto new_event = event.ToType<CollisionExitEvent>();
    }
    void onInputPressEvent(const Event<InputEvents>& event)
    {
        auto event_data = event.ToType<KeyPressEvent>();
        if(mKeyMap[event_data.key] == KeyAction::None)
        {
            //EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyStartEvent(event_data.key));
            mKeyMap[event_data.key] = KeyAction::Start;
        }
        else
        {
            mKeyMap[event_data.key] = KeyAction::Hold;
        }
    }
    void onInputReleaseEvent(const Event<InputEvents>& event)
    {
        auto event_data = event.ToType<KeyRelease>();
        if(mKeyMap[event_data.key] == KeyAction::Start || mKeyMap[event_data.key] == KeyAction::Hold)
        {
            //EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyEndEvent(event_data.key));
            mKeyMap[event_data.key] = KeyAction::End;
        }
        else
        {
            mKeyMap[event_data.key] = KeyAction::None;
            //this isnt working cause there isnt a new event being sent
        }
    }

    void processCollisionEesolutions()
    {
        for(auto& [key, value] : mCollisionMap)
        {
            value->resolveCollision();
            value->penetrationDepth = 0.0f;
        }
    }
    KeyAction getKeyAction(int key)
    {
        return mKeyMap[key];
    }
    void resetKeyStates()
    {
        for (auto& [key, action] : mKeyMap)
        {
            if (action == KeyAction::Start)
            {
                action = KeyAction::Hold;
            }
            else if (action == KeyAction::End)
            {
                action = KeyAction::None;
            }
        }
    }

public:
    template <typename T>
    void createSparseSet();

    template <typename T>
    void addComponent(unsigned short id, const T& component_data);


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

template <typename T>
bool Registry::hasSparseSet() const
{
    return mSparseSets.find(std::type_index(typeid(T))) != mSparseSets.end();
}

template <class T>
void Registry::createSparseSet()
{
    assert(mSparseSets.find(std::type_index(typeid(T))) == mSparseSets.end() &&
        "Error: Sparse set already exists for this type.");
    mSparseSets[std::type_index(typeid(T))] = std::make_unique<SparseSet<T>>();
}

template <typename T>
SparseSet<T>& Registry::getSparseSet() const
{
    assert(mSparseSets.find(std::type_index(typeid(T))) != mSparseSets.end() &&
        "Error: Sparse set does not exist for this type.");
    return *static_cast<SparseSet<T>*>(mSparseSets.at(std::type_index(typeid(T))).get());
}

template <typename T>
void Registry::addComponent(unsigned short id, const T& component_data)
{
    assert(mEntities.end() != std::find(mEntities.begin(), mEntities.end(), id) && "Entity does not exist.");
    auto& set = getSparseSet<T>();
    mCommandQueue.push(std::make_unique<AddComponentCommand<T>>(set, id, component_data));
}

template <typename T>
T& Registry::getComponent(unsigned short id) const
{
    assert(hasComponent<T>(id) && "Entity does not have component.");
    return getSparseSet<T>().get_item(id);
}

template <typename... T>
std::vector<unsigned short> Registry::getEntityIDs() const
{
    std::vector<unsigned short> result;
    if (sizeof...(T) == 0)
    {
        for (unsigned short i = 0; i < settings::max_entities; ++i)
        {
            if (std::find(mEntities.begin(), mEntities.end(), i) != mEntities.end())
            {
                result.emplace_back(i);
            }
        }
        return result;
    }
    std::vector<ISparseSet*> sparse_sets = {&getSparseSet<T>()...};

    // Initialize result with the first component's IDs if available
    result = sparse_sets[0]->getIDs();

    // Find intersection across all component types
    for (size_t i = 1; i < sparse_sets.size(); ++i)
    {
        result = sparse_sets[i]->getIntersection(result);
    }

    return result;
}

template <typename T>
bool Registry::hasComponent(unsigned short id) const
{
    return getSparseSet<T>().hasItem(id);
}

template <typename T>
void Registry::removeComponent(unsigned short id)
{
    assert(mEntities.end() != std::find(mEntities.begin(), mEntities.end(), id) && "Entity does not exist.");
    assert(hasComponent<T>(id) && "Entity does not have component.");
    auto& set = getSparseSet<T>();
    mCommandQueue.push(std::make_unique<RemoveComponentCommand<T>>(set, id));
}
