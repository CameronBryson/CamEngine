#pragma once
#include "User/Events/FactoryEvents.hpp"
#include <vector>
#include <entt/entity/fwd.hpp>
class BaseScene;
class Factory
{
public:
	explicit Factory(BaseScene* scene);
	~Factory();
	entt::entity createDirectionalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) const;
private:
	BaseScene* m_Scene;

};
