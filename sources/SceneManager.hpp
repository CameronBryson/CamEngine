#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "IScene.hpp"
#include <memory>
class SceneManager{
public:
    ~SceneManager();
    static SceneManager* GetInstance();
    void Init();
    void Update(float dt);
    void Render();
    void Shutdown();
    //idk if I should be using template
    //better solution is probably to pass interface
    template<typename T>
    void LoadScene(){
        if(m_currentScene!=nullptr)
            Shutdown();
        m_currentScene = std::unique_ptr<IScene>(new T());
    }
protected:
    SceneManager();
private:
    static SceneManager* m_instance;
    std::unique_ptr<IScene> m_currentScene;


};

#endif // SCENEMANAGER_HPP
