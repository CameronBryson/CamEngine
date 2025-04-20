#include "pch.hpp"
#include "TestScene.hpp"

#include <BaseScene.hpp>
#include <Components.hpp>
#include <entt/entity/registry.hpp>
#include <glm/ext/vector_float3.hpp>
#include "EngineUtil.hpp"
#include "GraphicsManager.hpp"
#include <GameManager.hpp>
TestScene::TestScene() : BaseScene() {}
TestScene::~TestScene() {}
void TestScene::init()
{
	BaseScene::init();

	auto controller = mEnttRegistry.create();


	auto sceneRoot = mEnttRegistry.create();
	mEnttRegistry.emplace<CTransform>(sceneRoot, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 1, 1, 1 });
	mEnttRegistry.emplace<CChildren>(sceneRoot);




	auto testParent = mEnttRegistry.create();
	mEnttRegistry.emplace<CTransform>(testParent, glm::vec3{ 0, 0, 0 }, glm::quat(), glm::vec3{ 1, 1, 1 });
	mEnttRegistry.emplace<CParent>(testParent, sceneRoot);
	mEnttRegistry.emplace<CChildren>(testParent);
	mEnttRegistry.get<CChildren>(sceneRoot).children.push_back(testParent);

	auto testModel = mEnttRegistry.create();
	mEnttRegistry.emplace<CModel>(testModel, "Helmet");
	glm::vec3 rotationEulerAngles = glm::radians(glm::vec3(0.0f, 0.0f, 180.0f)); // Adjust angles as needed
	glm::quat rotationQuat = glm::quat(rotationEulerAngles);
	mEnttRegistry.emplace<CTransform>(testModel, glm::vec3{ 0, 0, -0.5 }, rotationQuat, glm::vec3{ 1, 1, 1 });
	mEnttRegistry.emplace<CParent>(testModel, testParent);
	mEnttRegistry.get<CChildren>(testParent).children.push_back(testModel);

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
	glm::vec3 pointDiffuse = glm::vec3(2);    // Strong but not max diffuse
	glm::vec3 pointSpecular = glm::vec3(2);   // Full specular

	// Adjust attenuation for your scene scale (your chess piece is at -75 units)
	float constant = 1.0f;
	float linear = 0.22f;      // Reduced for larger scenes
	float quadratic = 0.20f; // Reduced for larger scenes

	//Position the light higher and further back to better illuminate the chess piece
	mEnttRegistry.emplace<CPointLight>(pointLight,
									   glm::vec3(9.0f, -3.0f, -3.5f), // Positioned above and closer to the chess piece
									   pointAmbient, pointDiffuse, pointSpecular,
									   constant, linear, quadratic);

	// For the spot light, adjust similarly
	//auto spotLight = mEnttRegistry.create();
	//glm::vec3 spotDirection = glm::normalize(glm::vec3(0.7f, -0.6f, 0.3f)); // Angle it slightly
	//float innerCutoff = glm::cos(glm::radians(12.5f));  // Use cos for better precision
	//float outerCutoff = glm::cos(glm::radians(17.5f));  // Use cos for better precision
	//float spotIntensity = 7.0f;
	//glm::vec3 spotColor = glm::vec3(1, 0.5, 1.0);
	//glm::vec3 spotAmbient = glm::vec3(0.0);
	//glm::vec3 spotDiffuse = spotIntensity * spotColor;
	//glm::vec3 spotSpecular = spotIntensity * spotColor;

	//mEnttRegistry.emplace<CSpotLight>(spotLight,
	//	glm::vec3(4.0f, 3.0f, 0.0f), // Position it above the chess piece
	//	spotDirection, spotAmbient, spotDiffuse, spotSpecular,
	//	constant, linear, quadratic,
	//	innerCutoff, outerCutoff);

	glm::vec3 directionalDirection = glm::normalize(glm::vec3(-0.35, -1.0f, -0.2f)); // Angle it slightly
	float directionalIntensity = 5.0f;
	glm::vec3 directionalColor = glm::vec3(0.95, 0.73, 0.44);
	glm::vec3 directionalAmbient = glm::vec3(0.0f);
	glm::vec3 directionalDiffuse = directionalColor * directionalIntensity;
	glm::vec3 directionalSpecular = directionalColor * directionalIntensity;

	auto directionalLight = mEnttRegistry.create();
	mEnttRegistry.emplace<CDirectionalLight>(directionalLight, directionalDirection, directionalAmbient, directionalDiffuse, directionalSpecular);
	double mouseX, mouseY;
	glfwGetCursorPos(GameManager::getGLFWWindow(), &mouseX, &mouseY);
	mLastMouseX = mouseX;
	mLastMouseY = mouseY;
	glfwSetInputMode(GameManager::getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}
void TestScene::lateInit() { BaseScene::lateInit(); }
void TestScene::update(float dt)
{
	BaseScene::update(dt);

	// Handle camera movement
	float finalSpeed = mMoveSpeed;
	if (engine_util::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
	{
		finalSpeed *= 2.0f; // Unity-like fast movement with shift
	}

	// WASD movement (horizontal and forward/backward)
	if (engine_util::isKeyPressed(GLFW_KEY_W))
	{
		mCurrentCamera.mPosition += mCurrentCamera.mFront * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_S))
	{
		mCurrentCamera.mPosition -= mCurrentCamera.mFront * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_A))
	{
		mCurrentCamera.mPosition -= mCurrentCamera.mRight * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_D))
	{
		mCurrentCamera.mPosition += mCurrentCamera.mRight * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_R))
	{
		GameManager::mGraphicsManager->reloadShaders();
	}

	// Q/E for vertical movement (Unity-like)
	if (engine_util::isKeyPressed(GLFW_KEY_E) || engine_util::isKeyPressed(GLFW_KEY_SPACE))
	{
		mCurrentCamera.mPosition += mCurrentCamera.mWorldUp * finalSpeed * dt;
	}
	if (engine_util::isKeyPressed(GLFW_KEY_Q))
	{
		mCurrentCamera.mPosition -= mCurrentCamera.mWorldUp * finalSpeed * dt;
	}
	// Arrow key camera rotation
	float rotationSpeed = 100.0f * dt; // Adjust this value to change rotation speed
	if (engine_util::isKeyPressed(GLFW_KEY_LEFT))
	{
		mCurrentCamera.mYaw -= rotationSpeed;
		mCurrentCamera.updateCamera();
	}
	if (engine_util::isKeyPressed(GLFW_KEY_RIGHT))
	{
		mCurrentCamera.mYaw += rotationSpeed;
		mCurrentCamera.updateCamera();
	}
	if (engine_util::isKeyPressed(GLFW_KEY_UP))
	{
		mCurrentCamera.mPitch += rotationSpeed;
		// Constrain pitch to prevent camera flipping
		if (mCurrentCamera.mPitch > 89.0f)
			mCurrentCamera.mPitch = 89.0f;
		mCurrentCamera.updateCamera();
	}
	if (engine_util::isKeyPressed(GLFW_KEY_DOWN))
	{
		mCurrentCamera.mPitch -= rotationSpeed;
		// Constrain pitch to prevent camera flipping
		if (mCurrentCamera.mPitch < -89.0f)
			mCurrentCamera.mPitch = -89.0f;
		mCurrentCamera.updateCamera();
	}
	// Handle camera rotation with right mouse button
	if (engine_util::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		// Hide cursor when right mouse is pressed
		glfwSetInputMode(GameManager::getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double mouseX, mouseY;
		glfwGetCursorPos(GameManager::getGLFWWindow(), &mouseX, &mouseY);

		if (!mIsRightMousePressed)
		{
			// First frame of right click, just update last position
			mIsRightMousePressed = true;
			mLastMouseX = mouseX;
			mLastMouseY = mouseY;
		}
		else
		{
			// Calculate mouse movement
			float xoffset = static_cast<float>(mouseX - mLastMouseX);
			float yoffset = static_cast<float>(mLastMouseY - mouseY); // Reversed: y coordinates are bottom-to-top

			mLastMouseX = mouseX;
			mLastMouseY = mouseY;

			// Apply sensitivity
			xoffset *= mMouseSensitivity;
			yoffset *= mMouseSensitivity;

			// Update camera angles
			mCurrentCamera.mYaw += xoffset;
			mCurrentCamera.mPitch += yoffset;

			// Constrain pitch to prevent camera flipping
			if (mCurrentCamera.mPitch > 89.0f)
				mCurrentCamera.mPitch = 89.0f;
			if (mCurrentCamera.mPitch < -89.0f)
				mCurrentCamera.mPitch = -89.0f;

			// Update camera vectors based on new angles
			mCurrentCamera.updateCamera();
		}
	}
	else
	{
		// Show cursor when right mouse is released
		if (mIsRightMousePressed)
		{
			glfwSetInputMode(GameManager::getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mIsRightMousePressed = false;
		}
	}
}
void TestScene::lateUpdate(float dt) { BaseScene::lateUpdate(dt); }
void TestScene::render(float dt) { BaseScene::render(dt); }
void TestScene::lateRender()
{
	BaseScene::lateRender();
}
void TestScene::shutdown()
{
	BaseScene::shutdown();
}
void TestScene::lateShutdown() { BaseScene::lateShutdown(); }

