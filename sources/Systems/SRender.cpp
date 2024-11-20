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
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

void SRender::init(GraphicsManager& graphics_manager)
{
	printf("Render init\n");
	loadShaders(graphics_manager);

	graphics_manager.loadMtl("assets/Default.mtl");
	graphics_manager.createModelFromObj("assets/Ship.obj", "player");

	graphics_manager.createModelFromObj("assets/sphere.obj", "sphere");
	graphics_manager.createModelFromObj("assets/cube.obj", "cube");
	graphics_manager.createModelFromObj("assets/quad.obj", "quad");
	graphics_manager.createModelFromObj("assets/skybox.obj", "skybox");
	graphics_manager.createModelFromObj("assets/asteroid.obj", "asteroid");
	graphics_manager.createModelFromObj("assets/sat.obj", "sat");
	graphics_manager.createModelFromObj("assets/enemy_ship.obj", "enemy");

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Enable backface culling
	glEnable(GL_CULL_FACE);

	// Specify that we want to cull back faces
	glCullFace(GL_BACK);

	// Optionally, specify the front face winding order (default is GL_CCW)
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
}

void SRender::update(const Registry & registry, GraphicsManager& graphics_manager,  Camera & camera)
{
	OpenGlUtil::clearBackground();

	auto & view_matrix = camera.GetViewMatrix();
	auto & proj_matrix = camera.GetProjectionMatrix();
	auto & transforms = registry.getSparseSet<CTransform>();
	auto & texture_shader_3D = graphics_manager.getShader("3D_texture");
	auto & color_shader_3D = graphics_manager.getShader("3D_color");
	auto & color_shader_2D = graphics_manager.getShader("2D_color");
	auto & texture_shader_2D = graphics_manager.getShader("2D_texture");
	auto & direction_lights = registry.getSparseSet<CDirectionalLight>();
	auto & point_lights = registry.getSparseSet<CPointLight>();
	const auto direction_light_ids = registry.getEntityIDs<CDirectionalLight>();
	const auto point_light_ids = registry.getEntityIDs<CPointLight,CTransform>();


	glm::mat4 ortho_projection = glm::ortho(-settings::aspect_ratio, settings::aspect_ratio, -1.0f, 1.0f, -1.0f, 1.0f);


	glEnable(GL_DEPTH_TEST);


	texture_shader_3D.use();
	texture_shader_3D.setMat4("projection", proj_matrix);
	texture_shader_3D.setMat4("view", view_matrix);
	texture_shader_3D.setInt("numDirLights", direction_light_ids.size());
	texture_shader_3D.setInt("numPointLights", point_light_ids.size());
	texture_shader_3D.setVec3("viewPos", camera.Position);

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
	drawModels(registry, graphics_manager, transforms, texture_shader_3D);
//#ifdef _DEBUG
	color_shader_3D.use();
	color_shader_3D.setMat4("projection", proj_matrix);
	color_shader_3D.setMat4("view", view_matrix);
	drawColliders(registry, graphics_manager, transforms, color_shader_3D);
//#endif



	color_shader_2D.use();

	color_shader_2D.setMat4("projection", ortho_projection);
	color_shader_2D.setMat4("view", glm::mat4(1.0f));
	drawUi(registry, graphics_manager, transforms, color_shader_2D);
}

void SRender::shutdown()
{
}

void SRender::drawModels(const Registry & registry,GraphicsManager& graphics_manager, SparseSet<CTransform> & transforms, ShaderProgram & shader)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	auto& models = registry.getSparseSet<CModel>();
	for( auto id : registry.getEntityIDs<CModel, CTransform,CBackground>() )
	{
		auto & model = models.get_item(id);
		auto & transform = transforms.get_item(id);
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
			glm::scale(glm::mat4(1.0f), transform.scale) *
			glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		shader.setMat4("model", model_matrix);
		graphics_manager.getModel(model.name).draw(shader,graphics_manager);
	}
	for( auto id : registry.getEntityIDs<CModel, CTransform>() )
	{
		auto & model = models.get_item(id);
		auto & transform = transforms.get_item(id);
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
			glm::scale(glm::mat4(1.0f), transform.scale) *
			glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		shader.setMat4("model", model_matrix);
		graphics_manager.getModel(model.name).draw(shader,graphics_manager);
	}

}

void SRender::drawColliders(const Registry & registry, GraphicsManager& graphics_manager, SparseSet<CTransform> & transforms, ShaderProgram & shader)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	auto & quads = registry.getSparseSet<CBoxBounds>();
	auto & spheres = registry.getSparseSet<CSphereBounds>();
	for( auto id : registry.getEntityIDs<CCollider, CTransform>())
	{
		if( registry.hasComponent<CBoxBounds>(id) )
		{
			auto & quad = quads.get_item(id);
			auto & transform = transforms.get_item(id);
			glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
				glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z)
				* glm::scale(glm::mat4(1.0f), quad.extents * transform.scale);
			shader.setMat4("model", model_matrix);

			auto & mesh = graphics_manager.getMesh("Cube");
			mesh.draw(shader,graphics_manager);
		}
		else if( registry.hasComponent<CSphereBounds>(id) )
		{
			auto & sphere = spheres.get_item(id);
			auto & transform = transforms.get_item(id);
			glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
				glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z) *
				glm::scale(glm::mat4(1.0f), transform.scale * (sphere.radius*2));
			shader.setMat4("model", model_matrix);
			auto & mesh = graphics_manager.getMesh("Sphere");
			mesh.draw(shader,graphics_manager);
		}
	}
}

void SRender::drawUi(const Registry &registry, GraphicsManager& graphics_manager, SparseSet<CTransform> &transforms, ShaderProgram &shader) {
	glDisable(GL_DEPTH_TEST);

	auto &ui = registry.getSparseSet<CUI>();
	for (auto id : registry.getEntityIDs<CUI, CTransform>()) {
		auto &transform = transforms.get_item(id);
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
								 glm::scale(glm::mat4(1.0f), transform.scale) *
								 glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		shader.setMat4("model", model_matrix);
		auto &mesh = graphics_manager.getMesh("Quad");
		mesh.draw(shader,graphics_manager);
	}
}


void SRender::loadShaders(GraphicsManager& graphics_manager)
{
	graphics_manager.loadShader(
		"sources/Shaders/vertex.vs",
		"sources/Shaders/3D_texture.fs", "3D_texture");
	graphics_manager.loadShader(
			"sources/Shaders/vertex.vs",
			"sources/Shaders/3D_color.fs", "3D_color");
	graphics_manager.loadShader(
		"sources/Shaders/vertex.vs",
		"sources/Shaders/2D_color.fs", "2D_texture");
	graphics_manager.loadShader(
			"sources/Shaders/vertex.vs",
			"sources/Shaders/2D_color.fs", "2D_color");
}
