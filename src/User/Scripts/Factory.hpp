#pragma once
#include <entt/entity/fwd.hpp>
#include <glm/ext/vector_float3.hpp>
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
