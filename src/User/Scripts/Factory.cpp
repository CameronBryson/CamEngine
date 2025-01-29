#include "pch.hpp"
#include "Factory.hpp"
#include "Components.hpp"
#include "BaseScene.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <glm/ext/vector_float3.hpp>

Factory::Factory(BaseScene* scene) : m_Scene(scene)
{

}

Factory::~Factory()
{

}

entt::entity Factory::createDirectionalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse,
									   const glm::vec3& specular) const
{
	const auto id = m_Scene->mEnttRegistry.create();
	m_Scene->mEnttRegistry.emplace<CDirectionalLight>(id, direction, ambient, diffuse, specular);
	return id;
}

entt::entity Factory::createPointLight(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float constant, const float linear, const float quadratic) const
{
	const auto id = m_Scene->mEnttRegistry.create();
	m_Scene->mEnttRegistry.emplace<CPointLight>(id, position, ambient, diffuse, specular, constant, linear, quadratic);
	return id;
}




