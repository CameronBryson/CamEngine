#include "PlayScene.hpp"
#include "Systems/PlayerController.hpp"

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

PlayScene::PlayScene() : mFactory(mRegistry)
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

    m_system_render_.init(mGraphicsManager);
    Timer benchmark_timer(Stats::stat_type::BENCHMARK);
    auto player  = mFactory.createPlayer(mRegistry);
    //m_registry_.addComponent<test>(player, test());
    mRegistry.addComponent<PlayerController>(player, &mCamera, &mRegistry, player);
    settings::player_id = player;

    mFactory.createBoundary(mRegistry, glm::vec3{-45,0,0}, glm::vec3{1,45,1});
    mFactory.createBoundary(mRegistry, glm::vec3{45,0,0}, glm::vec3{1,45,1});
    mFactory.createBoundary(mRegistry, glm::vec3{0,-45,0}, glm::vec3{45,1,1});
    mFactory.createBoundary(mRegistry, glm::vec3{0,45,0}, glm::vec3{45,1,1});

    auto skybox = mFactory.createSkybox(mRegistry, glm::vec3{0,0,0}, 10);
    mFactory.createDirectionalLight(mRegistry,glm::vec3{0,-0.2,-1.0}, glm::vec3{0.6,0.6,0.6}, glm::vec3{0.5f,0.5f,0.5f}, glm::vec3{0.2,0.2,0.2});

    mRegistry.processCommands();
    mCamera.camera_follow_target_ = &mRegistry.getComponent<CTransform>(player);
    mCamera.skybox_tranform = &mRegistry.getComponent<CTransform>(skybox);

    //EventHandler::GetInstance()->scriptDispatcher.SendEvent(ScriptEvents::Init);
    //EventHandler::GetInstance()->scriptDispatcher.SendEvent(ScriptEvents::LateInit);
}

void PlayScene::update(const float dt)
{
    Timer update_timer(Stats::stat_type::UPDATE);
    mRegistry.processCollisionEesolutions();
    m_system_camera_.update(mCamera, dt);
    //mSystemPlayer.update(mRegistry, mCamera, dt);
    m_system_enemy_.update(mRegistry, dt);
    m_system_ui_.update(mRegistry);
    m_system_asteroid_.update(mRegistry,dt);
    m_system_physics_.update(mRegistry, dt);
    m_system_health_.update(mRegistry, dt);
    m_system_boundry_.update(mRegistry,dt);
    m_system_wave_spawn_.update(mRegistry,dt);
    EventHandler::GetInstance()->scriptDispatcher.SendEvent(ScriptEvents::Update);

}

void PlayScene::lateUpdate(const float dt)
{
    m_system_collision_.update(mRegistry);
    mRegistry.processCommands();
    mRegistry.resetKeyStates();
    EventHandler::GetInstance()->scriptDispatcher.SendEvent(ScriptEvents::LateUpdate);
}

void PlayScene::render()
{
    Timer render_timer(Stats::stat_type::RENDER);
    m_system_render_.update(mRegistry, mGraphicsManager, mCamera);
}

void PlayScene::shutdown()
{
    m_system_render_.shutdown();
    mSystemPlayer.shutdown();
    m_system_physics_.shutdown();
    EventHandler::GetInstance()->scriptDispatcher.SendEvent(ScriptEvents::Shutdown);
}

Registry & PlayScene::getRegistry()
{
    return mRegistry;
}

void PlayScene::initSparseSets()
{
    mRegistry.createSparseSet<CTransform>();
    mRegistry.createSparseSet<CRender>();
    mRegistry.createSparseSet<CPlayer>();
    mRegistry.createSparseSet<CCollider>();
    mRegistry.createSparseSet<CHealth>();
    mRegistry.createSparseSet<CEnemy>();
    mRegistry.createSparseSet<CQuad>();
    mRegistry.createSparseSet<CSphere>();
    mRegistry.createSparseSet<CModel>();
    mRegistry.createSparseSet<CDirectionalLight>();
    mRegistry.createSparseSet<CDynamicBody>();
    mRegistry.createSparseSet<CUI>();
    mRegistry.createSparseSet<CDamage>();
    mRegistry.createSparseSet<CBackground>();
    mRegistry.createSparseSet<CAsteroid>();
    mRegistry.createSparseSet<CPointLight>();
    mRegistry.createSparseSet<CRepeatAcceleration>();
    mRegistry.createSparseSet<test>();
    mRegistry.createSparseSet<PlayerController>();
}