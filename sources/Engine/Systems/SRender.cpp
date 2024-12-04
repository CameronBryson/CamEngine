#include "SRender.hpp"

#include "Engine/Components.hpp"
#include "Engine/Util/EngineUtil.hpp"
#include "Engine/Util/platform.hpp"

#include "Engine/Util/OpenGLUtil.hpp"

#include "Engine/Managers/GraphicsManager.hpp"
#include <string>
#include "Engine/Base/BaseScene.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

SRender::SRender(BaseScene* scene) : mScene(scene)
{
}

void SRender::init()
{
	
	printf("Render init\n");
	loadShaders();
	//need to move all of this out of here and into a scene
	mScene->mGraphicsManager.loadFont("assets/arial.ttf", 48, "arial");
	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/spaceship_V1.obj"), "bottle");
	//mScene->mGraphicsManager.loadMtl("assets/Default.mtl");
	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/Ship.obj"), "player");

	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/sphere.obj"), "sphere");
	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/cube.obj"), "cube");
	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/quad.obj"), "quad");
	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/skybox.obj"), "skybox");
	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/asteroid.obj"), "asteroid");
	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/sat.obj"), "sat");
	mScene->mGraphicsManager.loadModel(engine_util::buildPath("assets/enemy_ship.obj"), "enemy");
	//mScene->mGraphicsManager.createModelFromObj("assets/Ship.obj", "player");

	/*mScene->mGraphicsManager.createModelFromObj("assets/sphere.obj", "sphere");
	mScene->mGraphicsManager.createModelFromObj("assets/cube.obj", "cube");
	mScene->mGraphicsManager.createModelFromObj("assets/quad.obj", "quad");
	mScene->mGraphicsManager.createModelFromObj("assets/skybox.obj", "skybox");
	mScene->mGraphicsManager.createModelFromObj("assets/asteroid.obj", "asteroid");
	mScene->mGraphicsManager.createModelFromObj("assets/sat.obj", "sat");
	mScene->mGraphicsManager.createModelFromObj("assets/enemy_ship.obj", "enemy");*/
	glClearColor(0, 0, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Enable backface culling
	glEnable(GL_CULL_FACE);

	// Specify that we want to cull back faces
	glCullFace(GL_BACK);

	// Optionally, specify the front face winding order (default is GL_CCW)
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
}



void SRender::render()
{
	OpenGlUtil::clearBackground();

	auto & view_matrix = mScene->mMainCamera.GetViewMatrix();
	auto & proj_matrix = mScene->mMainCamera.GetProjectionMatrix();
	auto & transforms = mScene->getSparseSet<CTransform>();
	auto  texture_shader_3D = mScene->mGraphicsManager.getShader("3D_texture");
	auto  color_shader_3D = mScene->mGraphicsManager.getShader("3D_color");
	auto  color_shader_2D = mScene->mGraphicsManager.getShader("2D_color");
	auto  texture_shader_2D = mScene->mGraphicsManager.getShader("2D_texture");
	auto & direction_lights = mScene->getSparseSet<CDirectionalLight>();
	auto & point_lights = mScene->getSparseSet<CPointLight>();
	const auto direction_light_ids = mScene->getEntityIDs<CDirectionalLight>();
	const auto point_light_ids = mScene->getEntityIDs<CPointLight,CTransform>();

	// Set up orthographic projection
	int windowWidth = settings::window_width;
	int windowHeight = settings::window_height;
	glm::mat4 ortho_projection = glm::ortho(
		0.0f, static_cast<float>(windowWidth),
		0.0f, static_cast<float>(windowHeight)
	);
	
	glEnable(GL_DEPTH_TEST);


	texture_shader_3D->use();
	texture_shader_3D->setMat4("projection", proj_matrix);
	texture_shader_3D->setMat4("view", view_matrix);
	texture_shader_3D->setInt("numDirLights", direction_light_ids.size());
	texture_shader_3D->setInt("numPointLights", point_light_ids.size());
	texture_shader_3D->setVec3("viewPos", mScene->mMainCamera.Position);

	for( int i = 0; i < direction_light_ids.size(); ++i )
	{
		auto & light = direction_lights.get_item(direction_light_ids[i]);
		std::string index = std::to_string(i);
		texture_shader_3D->setVec3("dirLights[" + index + "].direction", light.direction);
		texture_shader_3D->setVec3("dirLights[" + index + "].ambient", light.ambient);
		texture_shader_3D->setVec3("dirLights[" + index + "].diffuse", light.diffuse);
		texture_shader_3D->setVec3("dirLights[" + index + "].specular", light.specular);
	}
	for( int i = 0; i < point_light_ids.size(); ++i )
	{
		auto & light = point_lights.get_item(point_light_ids[i]);
		auto & transform = transforms.get_item(point_light_ids[i]);
		std::string index = std::to_string(i);
		texture_shader_3D->setVec3("pointLights[" + index + "].position", transform.position);
		texture_shader_3D->setVec3("pointLights[" + index + "].ambient", light.ambient);
		texture_shader_3D->setVec3("pointLights[" + index + "].diffuse", light.diffuse);
		texture_shader_3D->setVec3("pointLights[" + index + "].specular", light.specular);
		texture_shader_3D->setFloat("pointLights[" + index + "].constant", light.constant);
		texture_shader_3D->setFloat("pointLights[" + index + "].linear", light.linear);
		texture_shader_3D->setFloat("pointLights[" + index + "].quadratic", light.quadratic);

	}
	
	drawModels(transforms, *texture_shader_3D);
//#ifdef _DEBUG
	color_shader_3D->use();
	color_shader_3D->setMat4("projection", proj_matrix);
	color_shader_3D->setMat4("view", view_matrix);
	drawColliders(transforms, *color_shader_3D);
//#endif



	color_shader_2D->use();

	color_shader_2D->setMat4("projection", ortho_projection);
	color_shader_2D->setMat4("view", glm::mat4(1.0f));
	drawUi(transforms, *color_shader_2D);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto textShader = mScene->mGraphicsManager.getShader("text");
	textShader->use();
	textShader->setMat4("projection", ortho_projection);

	std::string text = "Test Hello World 123 ABC";
	float x = 200.0f; // x position in pixels
	float y = windowHeight - 100.0f; // Adjust y to start from top
	float scale = 1.0f;
	glm::vec3 textColor = { 0.3f, 0.5f, 0.7f };

	mScene->mGraphicsManager.getFont("arial")->renderText(*textShader, text, x, y, scale, textColor);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void SRender::shutdown()
{
}

void SRender::drawModels(SparseSet<CTransform> & transforms, const Shader & shader) const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	auto& models = mScene->getSparseSet<CModel>();
	for( auto id : mScene->getEntityIDs<CModel, CTransform,CBackground>() )
	{
		auto & model = models.get_item(id);
		auto & transform = transforms.get_item(id);
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
			glm::scale(glm::mat4(1.0f), transform.scale) *
			glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		shader.setMat4("model", model_matrix);
		mScene->mGraphicsManager.getModel(model.name)->draw(shader, mScene->mGraphicsManager);
	}
	for( auto id : mScene->getEntityIDs<CModel, CTransform>() )
	{
		auto & model = models.get_item(id);
		auto & transform = transforms.get_item(id);
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
			glm::scale(glm::mat4(1.0f), transform.scale) *
			glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		shader.setMat4("model", model_matrix);
		mScene->mGraphicsManager.getModel(model.name)->draw(shader,mScene->mGraphicsManager);
	}

}

void SRender::drawColliders(SparseSet<CTransform> & transforms, const Shader & shader) const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	auto & quads = mScene->getSparseSet<CBoxBounds>();
	auto & spheres = mScene->getSparseSet<CSphereBounds>();
	for( auto id : mScene->getEntityIDs<CCollider, CTransform>())
	{
		if(mScene->hasComponent<CBoxBounds>(id) )
		{
			auto & quad = quads.get_item(id);
			auto & transform = transforms.get_item(id);
			glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
				glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z)
				* glm::scale(glm::mat4(1.0f), quad.extents * transform.scale);
			shader.setMat4("model", model_matrix);

			auto mesh = mScene->mGraphicsManager.getMesh("Cube");
			mesh->draw(shader,mScene->mGraphicsManager);
		}
		else if(mScene->hasComponent<CSphereBounds>(id) )
		{
			auto & sphere = spheres.get_item(id);
			auto & transform = transforms.get_item(id);
			glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
				glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z) *
				glm::scale(glm::mat4(1.0f), transform.scale * (sphere.radius*2));
			shader.setMat4("model", model_matrix);
			auto  mesh = mScene->mGraphicsManager.getMesh("Sphere");
			mesh->draw(shader, mScene->mGraphicsManager);
		}
	}
}

void SRender::drawUi(SparseSet<CTransform> &transforms, const Shader &shader) const
{
	glDisable(GL_DEPTH_TEST);

	auto &ui = mScene->getSparseSet<CUI>();
	for (auto id : mScene->getEntityIDs<CUI, CTransform>()) {
		auto &transform = transforms.get_item(id);
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
								 glm::scale(glm::mat4(1.0f), transform.scale) *
								 glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		shader.setMat4("model", model_matrix);
		auto mesh = mScene->mGraphicsManager.getMesh("Quad");
		mesh->draw(shader,mScene->mGraphicsManager);
	}
}


void SRender::loadShaders() const
{

	mScene->mGraphicsManager.loadShader(
		"sources/Shaders/vertex.vert",
		"sources/Shaders/3D_texture.frag", "3D_texture");
	mScene->mGraphicsManager.loadShader(
			"sources/Shaders/vertex.vert",
			"sources/Shaders/3D_color.frag", "3D_color");
	mScene->mGraphicsManager.loadShader(
		"sources/Shaders/vertex.vert",
		"sources/Shaders/2D_color.frag", "2D_texture");
	mScene->mGraphicsManager.loadShader(
			"sources/Shaders/vertex.vert",
			"sources/Shaders/2D_color.frag", "2D_color");
	mScene->mGraphicsManager.loadShader(
		"sources/Shaders/text.vert",
		"sources/Shaders/text.frag", "text");
}

