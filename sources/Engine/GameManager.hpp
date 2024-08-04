#pragma once
#include "IScene.hpp"
class game_manager
{
  public:
    ~game_manager();
    static game_manager *get_instance();
    void init() const;
    void update(float dt) const;
    void render() const;
    void shutdown() const;
    // idk if I should be using template
    // better solution is probably to pass interface
    template <typename T> void load_scene()
    {
        if (m_current_scene_ != nullptr)
            shutdown();
        m_current_scene_ = new T();
    }

  protected:
    game_manager();

  private:
    static game_manager *m_instance_;
    i_scene *m_current_scene_;
};