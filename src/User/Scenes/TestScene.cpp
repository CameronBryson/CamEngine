#include "pch.hpp"
#include "TestScene.hpp"

#include "User/Scripts/Controllers/CameraController.hpp"
#include "User/Scripts/Factory.hpp"
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
	Factory factory(this);
	BaseScene::init();
	edyn::set_gravity(mEnttRegistry, edyn::vector3{ 0, -9.81, 0 });
	auto controller = mEnttRegistry.create();
	mEnttRegistry.emplace<CameraController>(controller, this, controller);


	auto sceneRoot = mEnttRegistry.create();
	mEnttRegistry.emplace<CTransform>(sceneRoot, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 1, 1, 1 });
	mEnttRegistry.emplace<CChildren>(sceneRoot);

	

	
	auto testParent = mEnttRegistry.create();
	mEnttRegistry.emplace<CTransform>(testParent, glm::vec3{ 0, 0, 0 }, glm::quat(), glm::vec3{ 1, 1, 1 });
	mEnttRegistry.emplace<CParent>(testParent, sceneRoot);
	mEnttRegistry.emplace<CChildren>(testParent);
	mEnttRegistry.get<CChildren>(sceneRoot).children.push_back(testParent);

	auto testModel = mEnttRegistry.create();
	mEnttRegistry.emplace<CModel>(testModel, "scene");
	mEnttRegistry.emplace<CTransform>(testModel, glm::vec3{ 0, 0, 0 }, glm::quat(), glm::vec3{1, 1, 1});
	mEnttRegistry.emplace<CParent>(testModel, testParent);
	mEnttRegistry.get<CChildren>(testParent).children.push_back(testModel);
	auto rigidBody = edyn::rigidbody_def();
	rigidBody.presentation = true;
	rigidBody.shape = edyn::box_shape{ 1, 1, 1 };
	rigidBody.mass = 1;
	rigidBody.kind = edyn::rigidbody_kind::rb_dynamic;
	rigidBody.position = edyn::vector3{ 0, 0, -10 };
	rigidBody.gravity = edyn::vector3{ 0, -10, 0 };
	edyn::make_rigidbody(testModel, mEnttRegistry,  rigidBody);



	factory.createDirectionalLight(glm::vec3{ 0, -0.2, -1.0 }, glm::vec3{ 1.0, 1.0, 1.0 }, glm::vec3{ 0.5f, 0.5f, 0.5f },
		glm::vec3{ 0.2, 0.2, 0.2 });
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

