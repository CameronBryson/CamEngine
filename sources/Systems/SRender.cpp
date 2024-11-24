#include "SRender.hpp"

#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "Engine/Stats.hpp"
#include "Math/MathUtil.hpp"

#include "Graphics/OpenGLUtil.hpp"

#include <Graphics/GraphicsManager.hpp>
#include "Graphics/Model.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/ShaderProgram.hpp"
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <future>
#include "Engine/BaseScene.hpp";
SRender::SRender(BaseScene* scene) : mScene(scene)
{
}

void SRender::init()
{
	
	printf("Render init\n");
	loadShaders();

	
	mScene->mGraphicsManager.loadMtl("assets/Default.mtl");
	mScene->mGraphicsManager.createModelFromObj("assets/Ship.obj", "player");

	mScene->mGraphicsManager.createModelFromObj("assets/sphere.obj", "sphere");
	mScene->mGraphicsManager.createModelFromObj("assets/cube.obj", "cube");
	mScene->mGraphicsManager.createModelFromObj("assets/quad.obj", "quad");
	mScene->mGraphicsManager.createModelFromObj("assets/skybox.obj", "skybox");
	mScene->mGraphicsManager.createModelFromObj("assets/asteroid.obj", "asteroid");
	mScene->mGraphicsManager.createModelFromObj("assets/sat.obj", "sat");
	mScene->mGraphicsManager.createModelFromObj("assets/enemy_ship.obj", "enemy");

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
	auto & texture_shader_3D = mScene->mGraphicsManager.getShader("3D_texture");
	auto & color_shader_3D = mScene->mGraphicsManager.getShader("3D_color");
	auto & color_shader_2D = mScene->mGraphicsManager.getShader("2D_color");
	auto & texture_shader_2D = mScene->mGraphicsManager.getShader("2D_texture");
	auto & direction_lights = mScene->getSparseSet<CDirectionalLight>();
	auto & point_lights = mScene->getSparseSet<CPointLight>();
	const auto direction_light_ids = mScene->getEntityIDs<CDirectionalLight>();
	const auto point_light_ids = mScene->getEntityIDs<CPointLight,CTransform>();


	glm::mat4 ortho_projection = glm::ortho(-settings::aspect_ratio, settings::aspect_ratio, -1.0f, 1.0f, -1.0f, 1.0f);


	glEnable(GL_DEPTH_TEST);


	texture_shader_3D.use();
	texture_shader_3D.setMat4("projection", proj_matrix);
	texture_shader_3D.setMat4("view", view_matrix);
	texture_shader_3D.setInt("numDirLights", direction_light_ids.size());
	texture_shader_3D.setInt("numPointLights", point_light_ids.size());
	texture_shader_3D.setVec3("viewPos", mScene->mMainCamera.Position);

	for( int i = 0; i < direction_light_ids.size(); ++i )
	{
		auto & light = direction_lights.get_item(direction_light_ids[i]);
		std::string index = std::to_string(i);
		texture_shader_3D.setVec3("dirLights[" + index + "].direction", light.direction);
		texture_shader_3D.setVec3("dirLights[" + index + "].ambient", light.ambient);
		texture_shader_3D.setVec3("dirLights[" + index + "].diffuse", light.diffuse);
		texture_shader_3D.setVec3("dirLights[" + index + "].specular", light.specular);
	}
	for( int i = 0; i < point_light_ids.size(); ++i )
	{
		auto & light = point_lights.get_item(point_light_ids[i]);
		auto & transform = transforms.get_item(point_light_ids[i]);
		std::string index = std::to_string(i);
		texture_shader_3D.setVec3("pointLights[" + index + "].position", transform.position);
		texture_shader_3D.setVec3("pointLights[" + index + "].ambient", light.ambient);
		texture_shader_3D.setVec3("pointLights[" + index + "].diffuse", light.diffuse);
		texture_shader_3D.setVec3("pointLights[" + index + "].specular", light.specular);
		texture_shader_3D.setFloat("pointLights[" + index + "].constant", light.constant);
		texture_shader_3D.setFloat("pointLights[" + index + "].linear", light.linear);
		texture_shader_3D.setFloat("pointLights[" + index + "].quadratic", light.quadratic);

	}
	drawModels(transforms, texture_shader_3D);
//#ifdef _DEBUG
	color_shader_3D.use();
	color_shader_3D.setMat4("projection", proj_matrix);
	color_shader_3D.setMat4("view", view_matrix);
	drawColliders(transforms, color_shader_3D);
//#endif



	color_shader_2D.use();

	color_shader_2D.setMat4("projection", ortho_projection);
	color_shader_2D.setMat4("view", glm::mat4(1.0f));
	drawUi(transforms, color_shader_2D);
}

void SRender::shutdown()
{
}

void SRender::drawModels(SparseSet<CTransform> & transforms, ShaderProgram & shader)
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
		mScene->mGraphicsManager.getModel(model.name).draw(shader, mScene->mGraphicsManager);
	}
	for( auto id : mScene->getEntityIDs<CModel, CTransform>() )
	{
		auto & model = models.get_item(id);
		auto & transform = transforms.get_item(id);
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
			glm::scale(glm::mat4(1.0f), transform.scale) *
			glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		shader.setMat4("model", model_matrix);
		mScene->mGraphicsManager.getModel(model.name).draw(shader,mScene->mGraphicsManager);
	}

}

void SRender::drawColliders(SparseSet<CTransform> & transforms, ShaderProgram & shader)
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

			auto & mesh = mScene->mGraphicsManager.getMesh("Cube");
			mesh.draw(shader,mScene->mGraphicsManager);
		}
		else if(mScene->hasComponent<CSphereBounds>(id) )
		{
			auto & sphere = spheres.get_item(id);
			auto & transform = transforms.get_item(id);
			glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
				glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z) *
				glm::scale(glm::mat4(1.0f), transform.scale * (sphere.radius*2));
			shader.setMat4("model", model_matrix);
			auto & mesh = mScene->mGraphicsManager.getMesh("Sphere");
			mesh.draw(shader, mScene->mGraphicsManager);
		}
	}
}

void SRender::drawUi(SparseSet<CTransform> &transforms, ShaderProgram &shader) {
	glDisable(GL_DEPTH_TEST);

	auto &ui = mScene->getSparseSet<CUI>();
	for (auto id : mScene->getEntityIDs<CUI, CTransform>()) {
		auto &transform = transforms.get_item(id);
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
								 glm::scale(glm::mat4(1.0f), transform.scale) *
								 glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		shader.setMat4("model", model_matrix);
		auto &mesh = mScene->mGraphicsManager.getMesh("Quad");
		mesh.draw(shader,mScene->mGraphicsManager);
	}
}


void SRender::loadShaders()
{
	mScene->mGraphicsManager.loadShader(
		"sources/Shaders/vertex.vs",
		"sources/Shaders/3D_texture.fs", "3D_texture");
	mScene->mGraphicsManager.loadShader(
			"sources/Shaders/vertex.vs",
			"sources/Shaders/3D_color.fs", "3D_color");
	mScene->mGraphicsManager.loadShader(
		"sources/Shaders/vertex.vs",
		"sources/Shaders/2D_color.fs", "2D_texture");
	mScene->mGraphicsManager.loadShader(
			"sources/Shaders/vertex.vs",
			"sources/Shaders/2D_color.fs", "2D_color");
}
