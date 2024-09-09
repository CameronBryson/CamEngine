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
#include "Event.hpp"
#include "EventHandler.hpp"

class registry
{
public:
    registry()
    {
        printf("Registry created\n");
        m_entities_.reserve(settings::max_entities);
        m_free_i_ds_.resize(settings::max_entities - 1);
        std::iota(m_free_i_ds_.begin(), m_free_i_ds_.end(), 1);
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::Enter, std::bind(&registry::on_collision_event, this, std::placeholders::_1));
        EventHandler::GetInstance()->collision_dispatcher.AddListener(CollisionEvents::Exit, std::bind(&registry::on_collision_event, this, std::placeholders::_1));

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
    void on_collision_event(const Event<CollisionEvents>& event)
    {
        if (event.GetType() == CollisionEvents::Enter)
        {
            collision_queue_.push(event.ToType<CollisionEnterEvent>().manifold);
        }
        else if (event.GetType() == CollisionEvents::Exit)
        {
            printf("Collision Exit\n");
        }
    }

    void process_collision_resolutions()
    {
        while (!collision_queue_.empty())
        {
            auto& manifold = collision_queue_.front();
            manifold.resolve_collision();
            EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionExitEvent(manifold));
            collision_queue_.pop();
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
    std::queue<collision_manifold> collision_queue_;
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
