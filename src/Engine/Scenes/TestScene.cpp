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
#include "EngineUtil.hpp"
#include <GameManager.hpp>
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
	//glm::vec3 rotationEulerAngles = glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust angles as needed
	//glm::quat rotationQuat = glm::quat(rotationEulerAngles);
	//mEnttRegistry.emplace<CTransform>(testModel, glm::vec3{ 0, 0, -2.5}, rotationQuat, glm::vec3{1, 1, 1});
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
	glm::vec3 pointAmbient = glm::vec3(0.05);
	glm::vec3 pointDiffuse = glm::vec3(0.5f);    // Strong but not max diffuse
	glm::vec3 pointSpecular = glm::vec3(0.6f);   // Full specular

	// Adjust attenuation for your scene scale (your chess piece is at -75 units)
	float constant = 1.0f;
	float linear = 0.09f;      // Reduced for larger scenes
	float quadratic = 0.0032f; // Reduced for larger scenes

	 //Position the light higher and further back to better illuminate the chess piece
	mEnttRegistry.emplace<CPointLight>(pointLight,
		glm::vec3(-9.5f, 0.5f, 3.0f), // Positioned above and closer to the chess piece
		pointAmbient, pointDiffuse, pointSpecular,
		constant, linear, quadratic);

	// For the spot light, adjust similarly
	auto spotLight = mEnttRegistry.create();
	glm::vec3 spotDirection = glm::normalize(glm::vec3(0.7f, -0.8f, 0.3f)); // Angle it slightly
	float innerCutoff = glm::cos(glm::radians(12.5f));  // Use cos for better precision
	float outerCutoff = glm::cos(glm::radians(17.5f));  // Use cos for better precision
	glm::vec3 spotAmbient = glm::vec3(0.01);
	glm::vec3 spotDiffuse = glm::vec3(0.7);
	glm::vec3 spotSpecular = glm::vec3(0.8);

	mEnttRegistry.emplace<CSpotLight>(spotLight,
		glm::vec3(4.0f, 3.0f, 0.0f), // Position it above the chess piece
		spotDirection, spotAmbient, spotDiffuse, spotSpecular,
		constant, linear, quadratic,
		innerCutoff, outerCutoff);

	glm::vec3 directionalDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -0.2f)); // Angle it slightly
	glm::vec3 directionalAmbient = glm::vec3(0.0f);
	glm::vec3 directionalDiffuse = glm::vec3(0.8f);
	glm::vec3 directionalSpecular = glm::vec3(0.9);

	auto directionalLight = mEnttRegistry.create();
	mEnttRegistry.emplace<CDirectionalLight>(directionalLight, directionalDirection, directionalAmbient, directionalDiffuse, directionalSpecular);
	double mouseX, mouseY;
	glfwGetCursorPos(GameManager::get_glfw_window(), &mouseX, &mouseY);
	mLastMouseX = mouseX;
	mLastMouseY = mouseY;
	glfwSetInputMode(GameManager::get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}
void TestScene::lateInit() { BaseScene::lateInit(); }
void TestScene::update(float dt) {
	BaseScene::update(dt);

	// Handle camera movement
	float finalSpeed = mMoveSpeed;
	if (engine_util::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		finalSpeed *= 2.0f; // Unity-like fast movement with shift
	}

	// WASD movement (horizontal and forward/backward)
	if (engine_util::isKeyPressed(GLFW_KEY_W)) {
		mCurrentCamera.Position += mCurrentCamera.Front * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_S)) {
		mCurrentCamera.Position -= mCurrentCamera.Front * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_A)) {
		mCurrentCamera.Position -= mCurrentCamera.Right * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_D)) {
		mCurrentCamera.Position += mCurrentCamera.Right * finalSpeed * dt;
	}

	// Q/E for vertical movement (Unity-like)
	if (engine_util::isKeyPressed(GLFW_KEY_E) || engine_util::isKeyPressed(GLFW_KEY_SPACE)) {
		mCurrentCamera.Position += mCurrentCamera.WorldUp * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_Q)) {
		mCurrentCamera.Position -= mCurrentCamera.WorldUp * finalSpeed * dt;
	}
	// Arrow key camera rotation
	float rotationSpeed = 100.0f * dt; // Adjust this value to change rotation speed
	if (engine_util::isKeyPressed(GLFW_KEY_LEFT)) {
		mCurrentCamera.Yaw -= rotationSpeed;
		mCurrentCamera.updateCameraVectors();
	}
	if (engine_util::isKeyPressed(GLFW_KEY_RIGHT)) {
		mCurrentCamera.Yaw += rotationSpeed;
		mCurrentCamera.updateCameraVectors();
	}
	if (engine_util::isKeyPressed(GLFW_KEY_UP)) {
		mCurrentCamera.Pitch += rotationSpeed;
		// Constrain pitch to prevent camera flipping
		if (mCurrentCamera.Pitch > 89.0f)
			mCurrentCamera.Pitch = 89.0f;
		mCurrentCamera.updateCameraVectors();
	}
	if (engine_util::isKeyPressed(GLFW_KEY_DOWN)) {
		mCurrentCamera.Pitch -= rotationSpeed;
		// Constrain pitch to prevent camera flipping
		if (mCurrentCamera.Pitch < -89.0f)
			mCurrentCamera.Pitch = -89.0f;
		mCurrentCamera.updateCameraVectors();
	}
	// Handle camera rotation with right mouse button
	if (engine_util::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
		// Hide cursor when right mouse is pressed
		glfwSetInputMode(GameManager::get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double mouseX, mouseY;
		glfwGetCursorPos(GameManager::get_glfw_window(), &mouseX, &mouseY);

		if (!mIsRightMousePressed) {
			// First frame of right click, just update last position
			mIsRightMousePressed = true;
			mLastMouseX = mouseX;
			mLastMouseY = mouseY;
		}
		else {
			// Calculate mouse movement
			float xoffset = static_cast<float>(mouseX - mLastMouseX);
			float yoffset = static_cast<float>(mLastMouseY - mouseY); // Reversed: y coordinates are bottom-to-top

			mLastMouseX = mouseX;
			mLastMouseY = mouseY;

			// Apply sensitivity
			xoffset *= mMouseSensitivity;
			yoffset *= mMouseSensitivity;

			// Update camera angles
			mCurrentCamera.Yaw += xoffset;
			mCurrentCamera.Pitch += yoffset;

			// Constrain pitch to prevent camera flipping
			if (mCurrentCamera.Pitch > 89.0f)
				mCurrentCamera.Pitch = 89.0f;
			if (mCurrentCamera.Pitch < -89.0f)
				mCurrentCamera.Pitch = -89.0f;

			// Update camera vectors based on new angles
			mCurrentCamera.updateCameraVectors();
		}
	}
	else {
		// Show cursor when right mouse is released
		if (mIsRightMousePressed) {
			glfwSetInputMode(GameManager::get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mIsRightMousePressed = false;
		}
	}
}
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

