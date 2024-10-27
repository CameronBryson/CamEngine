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
struct hash_pair {
    std::size_t operator()(const std::pair<unsigned short, unsigned short>& p) const {
        auto hash1 = std::hash<unsigned short>{}(p.first);
        auto hash2 = std::hash<unsigned short>{}(p.second);
        // Ensure the order does not change the hash
        return hash1 ^ hash2 ^ std::hash<unsigned short>{}(std::min(p.first, p.second)) ^ std::hash<unsigned short>{}(std::max(p.first, p.second));
    }
};
class registry
{
public:

    registry()
    {
        printf("Registry created\n");
        m_entities_.reserve(settings::max_entities);
        m_free_i_ds_.resize(settings::max_entities - 1);
        std::iota(m_free_i_ds_.begin(), m_free_i_ds_.end(), 1);
        for (int i = 0; i < 512; ++i)
        {
            m_key_map[i] = KeyAction::None;
        }
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::Detected, std::bind(&registry::on_collision_detected_event, this, std::placeholders::_1));
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::NotDetected, std::bind(&registry::on_collision_not_detected_event, this, std::placeholders::_1));
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::Enter, std::bind(&registry::on_collision_enter_event, this, std::placeholders::_1));
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::Exit, std::bind(&registry::on_collision_exit_event, this, std::placeholders::_1));
        EventHandler::GetInstance()->input_dispatcher.AddListener(InputEvents::KeyPress, std::bind(&registry::on_input_press_event, this, std::placeholders::_1));
        EventHandler::GetInstance()->input_dispatcher.AddListener(InputEvents::KeyRelease, std::bind(&registry::on_input_release_event, this, std::placeholders::_1));

    };

    ~registry()
    {
        process_commands();
        printf("Registry destroyed\n");
    }

public:
    unsigned short create_entity()
    {
        assert(!m_free_i_ds_.empty() && "No more entities available.");
        const unsigned short id = m_free_i_ds_.back();
        m_free_i_ds_.pop_back();
        m_entities_.push_back(id);
        return id;
    };

    void delete_entity(unsigned short id)
    {
        command_queue_.push(std::make_unique<delete_entity_command>(m_sparse_sets_, m_free_i_ds_, m_entities_, id));
    };

    void process_commands()
    {
        while (!command_queue_.empty())
        {
            const auto& command = command_queue_.front();
            command->execute();
            command_queue_.pop();
        }
    }
    void on_collision_detected_event(const Event<CollisionEvents>& event)
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
        m_collision_map[pair] = std::make_unique<collision_manifold>(new_event.manifold);
    }
    void on_collision_not_detected_event(const Event<CollisionEvents> &event){
        auto new_event = event.ToType<CollisionNotDetectedEvent>();
        auto pair = std::make_pair(new_event.id1, new_event.id2);
        if(m_collision_map.contains(pair))
        {
            //printf("Collision Exit\n");
            EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionExitEvent(new_event.id1, new_event.id2));
            m_collision_map.erase(pair);
        }
    }
    void on_collision_enter_event(const Event<CollisionEvents> &event){
        auto new_event = event.ToType<CollisionEnterEvent>();
        bool has_health1 = has_component<c_health>(new_event.id1);
        bool has_health2 = has_component<c_health>(new_event.id2);
        bool has_damage1 = has_component<c_damage>(new_event.id1);
        bool has_damage2 = has_component<c_damage>(new_event.id2);

        if (has_health1 && has_damage2)
        {
            auto& health = get_component<c_health>(new_event.id1);
            auto& damage = get_component<c_damage>(new_event.id2);
            health.health-= damage.damage;
        }

        if (has_health2 && has_damage1)
        {
            auto& health = get_component<c_health>(new_event.id2);
            auto& damage = get_component<c_damage>(new_event.id1);
            health.health-= damage.damage;
        }
    }
    void on_collision_exit_event(const Event<CollisionEvents> &event){
        auto new_event = event.ToType<CollisionExitEvent>();
    }
    void on_input_press_event(const Event<InputEvents>& event)
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
    void on_input_release_event(const Event<InputEvents>& event)
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

    void process_collision_resolutions()
    {
        for(auto& [key, value] : m_collision_map)
        {
            value->resolve_collision();
            value->penetration_depth_ = 0.0f;
        }
    }
    KeyAction get_key_action(int key)
    {
        return m_key_map[key];
    }
    void reset_key_states()
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
    void create_sparse_set();

    template <typename T>
    void add_component(unsigned short id, const T& component_data);


    template <typename T>
    T& get_component(unsigned short id) const;

    template <typename T>
    [[nodiscard]] bool has_component(unsigned short id) const;

    template <typename T>
    void remove_component(unsigned short id);

    template <typename... T>
    [[nodiscard]] std::vector<unsigned short> get_entity_ids() const;

    template <typename T>
    sparse_set<T>& get_sparse_set() const;

    template <typename T>
    [[nodiscard]] bool has_sparse_set() const;

private:
    std::vector<unsigned short> m_free_i_ds_;
    std::vector<unsigned short> m_entities_;
    std::unordered_map<std::type_index, std::unique_ptr<i_sparse_set>> m_sparse_sets_;
    std::queue<std::unique_ptr<i_command>> command_queue_;
    std::unordered_map<std::pair<unsigned short, unsigned short>, std::unique_ptr<collision_manifold>,hash_pair> m_collision_map;
    std::unordered_map<int, KeyAction> m_key_map;
    //some kind of key storage
};

template <typename T>
bool registry::has_sparse_set() const
{
    return m_sparse_sets_.find(std::type_index(typeid(T))) != m_sparse_sets_.end();
}

template <class T>
void registry::create_sparse_set()
{
    assert(m_sparse_sets_.find(std::type_index(typeid(T))) == m_sparse_sets_.end() &&
        "Error: Sparse set already exists for this type.");
    m_sparse_sets_[std::type_index(typeid(T))] = std::make_unique<sparse_set<T>>();
}

template <typename T>
sparse_set<T>& registry::get_sparse_set() const
{
    assert(m_sparse_sets_.find(std::type_index(typeid(T))) != m_sparse_sets_.end() &&
        "Error: Sparse set does not exist for this type.");
    return *static_cast<sparse_set<T>*>(m_sparse_sets_.at(std::type_index(typeid(T))).get());
}

template <typename T>
void registry::add_component(unsigned short id, const T& component_data)
{
    assert(m_entities_.end() != std::find(m_entities_.begin(), m_entities_.end(), id) && "Entity does not exist.");
    auto& set = get_sparse_set<T>();
    command_queue_.push(std::make_unique<add_component_command<T>>(set, id, component_data));
}

template <typename T>
T& registry::get_component(unsigned short id) const
{
    assert(has_component<T>(id) && "Entity does not have component.");
    return get_sparse_set<T>().get_item(id);
}

template <typename... T>
std::vector<unsigned short> registry::get_entity_ids() const
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
    std::vector<i_sparse_set*> sparse_sets = {&get_sparse_set<T>()...};

    // Initialize result with the first component's IDs if available
    result = sparse_sets[0]->get_ids();

    // Find intersection across all component types
    for (size_t i = 1; i < sparse_sets.size(); ++i)
    {
        result = sparse_sets[i]->get_intersection(result);
    }

    return result;
}

template <typename T>
bool registry::has_component(unsigned short id) const
{
    return get_sparse_set<T>().has_item(id);
}

template <typename T>
void registry::remove_component(unsigned short id)
{
    assert(m_entities_.end() != std::find(m_entities_.begin(), m_entities_.end(), id) && "Entity does not exist.");
    assert(has_component<T>(id) && "Entity does not have component.");
    auto& set = get_sparse_set<T>();
    command_queue_.push(std::make_unique<remove_component_command<T>>(set, id));
}
