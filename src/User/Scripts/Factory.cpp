#include "pch.hpp"
#include "Factory.hpp"
#include "User/Events/FactoryEvents.hpp"
#include "Engine/Base/BaseScene.hpp"
#include "EventHandler.hpp"

Factory::Factory(BaseScene* scene) : m_Scene(scene)
{
	auto* eventHandler = EventHandler::GetInstance();

}

Factory::~Factory()
{
	auto* eventHandler = EventHandler::GetInstance();

}

entt::entity Factory::createDirectionalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse,
									   const glm::vec3& specular) const
{
	const auto id = m_Scene->mEnttRegistry.create();
	m_Scene->mEnttRegistry.emplace<CDirectionalLight>(id, direction, ambient, diffuse, specular);
	//const auto id = m_Scene->createEntity();
	//m_Scene->addComponent<CDirectionalLight>(id, direction, ambient, diffuse, specular);
	return id;
}




