#include "pch.hpp"
#include "TestScene.hpp"

#include <BaseScene.hpp>
#include <Components.hpp>
#include <edyn/math/vector3.hpp>
#include <edyn/shapes/box_shape.hpp>
#include <edyn/util/gravity_util.hpp>
#include <edyn/util/rigidbody.hpp>
#include <entt/entity/registry.hpp>
#include <glm/ext/vector_float3.hpp>
TestScene::TestScene() : BaseScene() {}
TestScene::~TestScene() {}
void TestScene::init()
{
	BaseScene::init();
	edyn::set_gravity(mEnttRegistry, edyn::vector3{ 0, -9.81, 0 });
	auto controller = mEnttRegistry.create();


	auto sceneRoot = mEnttRegistry.create();
	mEnttRegistry.emplace<CTransform>(sceneRoot, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 1, 1, 1 });
	mEnttRegistry.emplace<CChildren>(sceneRoot);

	

	
	auto testParent = mEnttRegistry.create();
	mEnttRegistry.emplace<CTransform>(testParent, glm::vec3{ 0, 0, 0 }, glm::quat(), glm::vec3{ 1, 1, 1 });
	mEnttRegistry.emplace<CParent>(testParent, sceneRoot);
	mEnttRegistry.emplace<CChildren>(testParent);
	mEnttRegistry.get<CChildren>(sceneRoot).children.push_back(testParent);

	//auto testModel = mEnttRegistry.create();
	//mEnttRegistry.emplace<CModel>(testModel, "MetalTests");
	//glm::vec3 rotationEulerAngles = glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f)); // Adjust angles as needed
	//glm::quat rotationQuat = glm::quat(rotationEulerAngles);
	//mEnttRegistry.emplace<CTransform>(testModel, glm::vec3{ 0, 0, -25 }, rotationQuat, glm::vec3{1, 1, 1});
	//mEnttRegistry.emplace<CParent>(testModel, testParent);
	//mEnttRegistry.get<CChildren>(testParent).children.push_back(testModel);

	auto testModel2 = mEnttRegistry.create();
	mEnttRegistry.emplace<CModel>(testModel2, "Chess");
	glm::vec3 rotationEulerAngles2 = glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust angles as needed
	glm::quat rotationQuat2 = glm::quat(rotationEulerAngles2);
	mEnttRegistry.emplace<CTransform>(testModel2, glm::vec3{ 0, -50, -75 }, rotationQuat2, glm::vec3{ 80, 80, 80 });
	mEnttRegistry.emplace<CParent>(testModel2, testParent);
	mEnttRegistry.get<CChildren>(testParent).children.push_back(testModel2);


	//auto rigidBody = edyn::rigidbody_def();
	//rigidBody.presentation = true;
	//rigidBody.shape = edyn::box_shape{ 1, 1, 1 };
	//rigidBody.mass = 1;
	//rigidBody.kind = edyn::rigidbody_kind::rb_dynamic;
	//rigidBody.position = edyn::vector3{ 0, 0, 0 };
	//rigidBody.gravity = edyn::vector3{ 0, 0, 0 };
	//edyn::make_rigidbody(testModel, mEnttRegistry,  rigidBody);

	auto pointLight = mEnttRegistry.create();
	glm::vec3 ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
	//mEnttRegistry.emplace<CPointLight>(pointLight, glm::vec3(0, 0, 0), ambient, diffuse, specular, constant, linear, quadratic);

	auto spotLight = mEnttRegistry.create();
	glm::vec3 direction = glm::vec3(0, -1, 0);
	float innerCutoff = glm::radians(12.5f);
	float outerCutoff = glm::radians(17.5f);
	//mEnttRegistry.emplace<CSpotLight>(spotLight, glm::vec3(0, 1, 0), direction, ambient, diffuse, specular, constant, linear, quadratic, innerCutoff, outerCutoff);

	auto directionalLight = mEnttRegistry.create();
	mEnttRegistry.emplace<CDirectionalLight>(directionalLight, direction, ambient, diffuse, specular);



}
void TestScene::lateInit() { BaseScene::lateInit(); }
void TestScene::update(float dt) { BaseScene::update(dt); }
void TestScene::lateUpdate(float dt) { BaseScene::lateUpdate(dt); }
void TestScene::render() { BaseScene::render(); }
void TestScene::lateRender()
{
	BaseScene::lateRender();
}
void TestScene::shutdown() 
{ 
	BaseScene::shutdown();
}
void TestScene::lateShutdown() { BaseScene::lateShutdown(); }

