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
	//auto controller = createEntity();
	//addComponent<CameraController>(controller, this, controller);

	auto controller = mEnttRegistry.create();
	mEnttRegistry.emplace<CameraController>(controller, this, controller);

	factory.createDirectionalLight(glm::vec3{0, -0.2, -1.0}, glm::vec3{1.0, 1.0, 1.0}, glm::vec3{0.5f, 0.5f, 0.5f},
	                               glm::vec3{0.2, 0.2, 0.2});

	auto testModel = mEnttRegistry.create();
	mEnttRegistry.emplace<CModel>(testModel, "bottle");
	auto rigidBody = edyn::rigidbody_def();
	rigidBody.presentation = true;
	rigidBody.shape = edyn::box_shape{ 1, 1, 1 };
	rigidBody.mass = 1;
	rigidBody.kind = edyn::rigidbody_kind::rb_dynamic;
	rigidBody.position = edyn::vector3{ 0, 0, -10 };
	rigidBody.gravity = edyn::vector3{ 0, 0, 0 };
	edyn::make_rigidbody(testModel, mEnttRegistry,  rigidBody);

	//mEnttRegistry.emplace<edyn::position>(testModel, edyn::vector3{ 0, 0, -10 });
	//mEnttRegistry.emplace<edyn::present_position>(testModel, edyn::vector3{ 0, 0, -10 });
	//mEnttRegistry.emplace<edyn::present_orientation>(testModel, edyn::quaternion());
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

