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
	mEnttRegistry.emplace<CModel>(testModel2, "Sponza");
	glm::vec3 rotationEulerAngles2 = glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust angles as needed
	glm::quat rotationQuat2 = glm::quat(rotationEulerAngles2);
	mEnttRegistry.emplace<CTransform>(testModel2, glm::vec3{ 0, -5, 0 }, rotationQuat2, glm::vec3(1.0f));
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

// In TestScene::init()
	auto pointLight = mEnttRegistry.create();
	// Add some ambient light and tone down the intensity
	glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);    // Soft ambient light
	glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);    // Strong but not max diffuse
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);   // Full specular

	// Adjust attenuation for your scene scale (your chess piece is at -75 units)
	float constant = 1.0f;
	float linear = 0.0014f;      // Reduced for larger scenes
	float quadratic = 0.000007f; // Reduced for larger scenes

	 //Position the light higher and further back to better illuminate the chess piece
	mEnttRegistry.emplace<CPointLight>(pointLight,
		glm::vec3(0.0f, 5.0f, 0.0f), // Positioned above and closer to the chess piece
		ambient, diffuse, specular,
		constant, linear, quadratic);

	// For the spot light, adjust similarly
	auto spotLight = mEnttRegistry.create();
	glm::vec3 direction = glm::normalize(glm::vec3(0.0f, -1.0f, -0.5f)); // Angle it slightly
	float innerCutoff = glm::cos(glm::radians(12.5f));  // Use cos for better precision
	float outerCutoff = glm::cos(glm::radians(17.5f));  // Use cos for better precision

	//mEnttRegistry.emplace<CSpotLight>(spotLight,
	//	glm::vec3(0.0f, 5.0f, 0.0f), // Position it above the chess piece
	//	direction, ambient, diffuse, specular,
	//	constant, linear, quadratic,
	//	innerCutoff, outerCutoff);


	auto directionalLight = mEnttRegistry.create();
	//mEnttRegistry.emplace<CDirectionalLight>(directionalLight, direction, ambient, diffuse, specular);



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

