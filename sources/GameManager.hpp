#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "IScene.hpp"
#include "ThreadPool.hpp"
#include <memory>
class GameManager{
public:
    ~GameManager();
    static GameManager* GetInstance();
    void Init() const;
    void Update(float dt) const;
    void Render() const;
    void Shutdown() const;
    [[nodiscard]] ThreadPool& GetThreadPool() const;
    //idk if I should be using template
    //better solution is probably to pass interface
    template<typename T>
    void LoadScene(){
        if(m_currentScene!=nullptr)
            Shutdown();
        m_currentScene = new T();
    }
protected:
    GameManager();
private:
    static GameManager* m_instance;
    IScene* m_currentScene;
    ThreadPool* m_threadPool;


};

#endif // SCENEMANAGER_HPP
