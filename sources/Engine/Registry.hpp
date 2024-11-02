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
        auto hash1 = std::hash<unsigned short>{}(p.first);
        auto hash2 = std::hash<unsigned short>{}(p.second);
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
        m_entities_.reserve(settings::max_entities);
        m_free_i_ds_.resize(settings::max_entities - 1);
        std::iota(m_free_i_ds_.begin(), m_free_i_ds_.end(), 1);
        for (int i = 0; i < 512; ++i)
        {
            m_key_map[i] = KeyAction::None;
        }
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::Detected, std::bind(&Registry::onCollisionDetectedEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::NotDetected, std::bind(&Registry::onCollisionNotDetectedEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::Enter, std::bind(&Registry::onCollisionEnterEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::Exit, std::bind(&Registry::onCollisionExitEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->input_dispatcher.AddListener(InputEvents::KeyPress, std::bind(&Registry::onInputPressEvent, this, std::placeholders::_1));
        EventHandler::GetInstance()->input_dispatcher.AddListener(InputEvents::KeyRelease, std::bind(&Registry::onInputReleaseEvent, this, std::placeholders::_1));

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
        assert(!m_free_i_ds_.empty() && "No more entities available.");
        const unsigned short id = m_free_i_ds_.back();
        m_free_i_ds_.pop_back();
        m_entities_.push_back(id);
        return id;
    };

    void deleteEntity(unsigned short id)
    {
        command_queue_.push(std::make_unique<DeleteEntityCommand>(m_sparse_sets_, m_free_i_ds_, m_entities_, id));
    };

    void processCommands()
    {
        while (!command_queue_.empty())
        {
            const auto& command = command_queue_.front();
            command->execute();
            command_queue_.pop();
        }
    }
    void onCollisionDetectedEvent(const Event<CollisionEvents>& event)
    {
        auto new_event = event.ToType<CollisionDetectedEvent>();
        auto pair = std::make_pair(new_event.id1, new_event.id2);
        if (!m_collision_map.contains(pair))
        {
            //printf("Collision Enter\n");
            EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionEnterEvent(new_event.id1, new_event.id2));
        }
        else
        {
            //printf("Collision Stay\n");
            EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionStayEvent(new_event.id1, new_event.id2));
        }
        m_collision_map[pair] = std::make_unique<CollisionManifold>(new_event.manifold);
    }
    void onCollisionNotDetectedEvent(const Event<CollisionEvents> &event){
        auto new_event = event.ToType<CollisionNotDetectedEvent>();
        auto pair = std::make_pair(new_event.id1, new_event.id2);
        if(m_collision_map.contains(pair))
        {
            //printf("Collision Exit\n");
            EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionExitEvent(new_event.id1, new_event.id2));
            m_collision_map.erase(pair);
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
        if(m_key_map[event_data.key] == KeyAction::None)
        {
            //EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyStartEvent(event_data.key));
            m_key_map[event_data.key] = KeyAction::Start;
        }
        else
        {
            m_key_map[event_data.key] = KeyAction::Hold;
        }
    }
    void onInputReleaseEvent(const Event<InputEvents>& event)
    {
        auto event_data = event.ToType<KeyRelease>();
        if(m_key_map[event_data.key] == KeyAction::Start || m_key_map[event_data.key] == KeyAction::Hold)
        {
            //EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyEndEvent(event_data.key));
            m_key_map[event_data.key] = KeyAction::End;
        }
        else
        {
            m_key_map[event_data.key] = KeyAction::None;
            //this isnt working cause there isnt a new event being sent
        }
    }

    void processCollisionEesolutions()
    {
        for(auto& [key, value] : m_collision_map)
        {
            value->resolveCollision();
            value->penetrationDepth = 0.0f;
        }
    }
    KeyAction getKeyAction(int key)
    {
        return m_key_map[key];
    }
    void resetKeyStates()
    {
        for (auto& [key, action] : m_key_map)
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
    std::vector<unsigned short> m_free_i_ds_;
    std::vector<unsigned short> m_entities_;
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> m_sparse_sets_;
    std::queue<std::unique_ptr<ICommand>> command_queue_;
    std::unordered_map<std::pair<unsigned short, unsigned short>, std::unique_ptr<CollisionManifold>,HashPair> m_collision_map;
    std::unordered_map<int, KeyAction> m_key_map;
    //some kind of key storage
};

template <typename T>
bool Registry::hasSparseSet() const
{
    return m_sparse_sets_.find(std::type_index(typeid(T))) != m_sparse_sets_.end();
}

template <class T>
void Registry::createSparseSet()
{
    assert(m_sparse_sets_.find(std::type_index(typeid(T))) == m_sparse_sets_.end() &&
        "Error: Sparse set already exists for this type.");
    m_sparse_sets_[std::type_index(typeid(T))] = std::make_unique<SparseSet<T>>();
}

template <typename T>
SparseSet<T>& Registry::getSparseSet() const
{
    assert(m_sparse_sets_.find(std::type_index(typeid(T))) != m_sparse_sets_.end() &&
        "Error: Sparse set does not exist for this type.");
    return *static_cast<SparseSet<T>*>(m_sparse_sets_.at(std::type_index(typeid(T))).get());
}

template <typename T>
void Registry::addComponent(unsigned short id, const T& component_data)
{
    assert(m_entities_.end() != std::find(m_entities_.begin(), m_entities_.end(), id) && "Entity does not exist.");
    auto& set = getSparseSet<T>();
    command_queue_.push(std::make_unique<AddComponentCommand<T>>(set, id, component_data));
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
            if (std::find(m_entities_.begin(), m_entities_.end(), i) != m_entities_.end())
            {
                result.push_back(i);
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
    assert(m_entities_.end() != std::find(m_entities_.begin(), m_entities_.end(), id) && "Entity does not exist.");
    assert(hasComponent<T>(id) && "Entity does not have component.");
    auto& set = getSparseSet<T>();
    command_queue_.push(std::make_unique<RemoveComponentCommand<T>>(set, id));
}
