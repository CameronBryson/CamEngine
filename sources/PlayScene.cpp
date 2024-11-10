#include "PlayScene.hpp"

#include <Engine/Event.hpp>

int created_counter = 0;
int destroyed_counter = 0;
int moved_counter = 0;
int copied_count = 0;
int equal_counter = 0;
struct test
{
    test()
    {
	created_counter++;
    }
    ~test()
    {
	destroyed_counter++;
    }
    test(test&& obj) : data(obj.data)
    {
	moved_counter++;
    }
    test(const test& obj)
    {
	copied_count++;
	this->data = obj.data;
    }
    test& operator=(const test&)
    {
	equal_counter++;
	return *this;
    }
    int data = 0;
};

PlayScene::PlayScene() : m_factory_(m_registry_)
{
    printf("PlayScene created\n");

}

PlayScene::~PlayScene()
{
    printf("PlayScene destroyed\n");
}

void PlayScene::init()
{

    initSparseSets();

    m_system_render_.init(m_graphics_manager_);
    Timer benchmark_timer(Stats::stat_type::BENCHMARK);
    auto player  = m_factory_.createPlayer(m_registry_);
    //m_registry_.addComponent<test>(player, test());
    m_registry_.getSparseSet<test>().addItem(player, test());
    settings::player_id = player;

    m_factory_.createBoundary(m_registry_, glm::vec3{-45,0,0}, glm::vec3{1,45,1});
    m_factory_.createBoundary(m_registry_, glm::vec3{45,0,0}, glm::vec3{1,45,1});
    m_factory_.createBoundary(m_registry_, glm::vec3{0,-45,0}, glm::vec3{45,1,1});
    m_factory_.createBoundary(m_registry_, glm::vec3{0,45,0}, glm::vec3{45,1,1});

    auto skybox = m_factory_.createSkybox(m_registry_, glm::vec3{0,0,0}, 10);
    m_factory_.createDirectionalLight(m_registry_,glm::vec3{0,-0.2,-1.0}, glm::vec3{0.2,0.2,0.2}, glm::vec3{0.3f,0.3f,0.3f}, glm::vec3{0.1,0.1,0.1});

    m_registry_.processCommands();
    m_camera_.camera_follow_target_ = &m_registry_.getComponent<CTransform>(player);
    m_camera_.skybox_tranform = &m_registry_.getComponent<CTransform>(skybox);
}

void PlayScene::update(const float dt)
{
    Timer update_timer(Stats::stat_type::UPDATE);
    m_registry_.processCollisionEesolutions();
    m_system_camera_.update(m_camera_, dt);
    m_system_player_.update(m_registry_, m_camera_, dt);
    m_system_enemy_.update(m_registry_, dt);
    m_system_ui_.update(m_registry_);
    m_system_asteroid_.update(m_registry_,dt);
    m_system_physics_.update(m_registry_, dt);
    m_system_health_.update(m_registry_, dt);
    m_system_boundry_.update(m_registry_,dt);
    m_system_wave_spawn_.update(m_registry_,dt);
}

void PlayScene::lateUpdate(const float dt)
{
    m_system_collision_.update(m_registry_);
    m_registry_.processCommands();
    m_registry_.resetKeyStates();
}

void PlayScene::render()
{
    Timer render_timer(Stats::stat_type::RENDER);
    m_system_render_.update(m_registry_, m_graphics_manager_, m_camera_);
}

void PlayScene::shutdown()
{
    m_system_render_.shutdown();
    m_system_player_.shutdown();
    m_system_physics_.shutdown();
}

Registry & PlayScene::getRegistry()
{
    return m_registry_;
}

void PlayScene::initSparseSets()
{
    m_registry_.createSparseSet<CTransform>();
    m_registry_.createSparseSet<CRender>();
    m_registry_.createSparseSet<CPlayer>();
    m_registry_.createSparseSet<CCollider>();
    m_registry_.createSparseSet<CHealth>();
    m_registry_.createSparseSet<CEnemy>();
    m_registry_.createSparseSet<CQuad>();
    m_registry_.createSparseSet<CSphere>();
    m_registry_.createSparseSet<CModel>();
    m_registry_.createSparseSet<CDirectionalLight>();
    m_registry_.createSparseSet<CDynamicBody>();
    m_registry_.createSparseSet<CUI>();
    m_registry_.createSparseSet<CDamage>();
    m_registry_.createSparseSet<CBackground>();
    m_registry_.createSparseSet<CAsteroid>();
    m_registry_.createSparseSet<CPointLight>();
    m_registry_.createSparseSet<CRepeatAcceleration>();
    m_registry_.createSparseSet<test>();
}