#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "glm/vec3.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Model.hpp"
#include "Engine/Graphics/Material.hpp"
#include "Engine/Graphics/Texture.hpp"
#include "Engine/Graphics/Mesh.hpp"
#include "Engine/Graphics/Font.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class GraphicsManager
{
public:
	GraphicsManager() = default;
	~GraphicsManager();

	// Load and generate a shader program from vertex and fragment shader source files
	std::shared_ptr<Shader> loadShader(const std::string& vShaderFile, const std::string& fShaderFile, const std::string& name);
	// Retrieve a stored shader
	std::shared_ptr<Shader> getShader(const std::string& name);

	// Load and generate a texture from a file
	std::shared_ptr<Texture> loadTexture(const std::string& file, aiTextureType type,  const std::string& name);
	// Retrieve a stored texture
	std::shared_ptr<Texture> getTexture(const std::string& name);

	// Create and store a mesh
	std::shared_ptr<Mesh> createMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices,const std::string& material_name);
	// Retrieve a stored mesh
	std::shared_ptr<Mesh> getMesh(const std::string& name);

	// Create and store a material
	std::shared_ptr<Material> createMaterial(const std::string& name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, float transparency, std::shared_ptr<Texture> ambientMap, std::shared_ptr<Texture> diffuseMap,
		std::shared_ptr<Texture> specularMap, std::shared_ptr<Texture> normalMap);
	// Retrieve a stored material
	std::shared_ptr<Material> getMaterial(const std::string& name);

	// Create and store a model
	std::shared_ptr<Model> createModel(const std::vector<std::string>& mesh_names, const std::string& name);
	// Retrieve a stored model
	std::shared_ptr<Model> getModel(const std::string& name);

	std::shared_ptr<Font> loadFont(const std::string& fontPath, float fontSize, const std::string& fontName);
	std::shared_ptr<Font> getFont(const std::string& name);

	void Clear();


	std::shared_ptr<Model> loadModel(const std::string& file, const std::string& name);

	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	std::string processMaterial(aiMaterial* material, const aiScene* scene);


private:
	// Maps to store shared pointers to resources
	std::unordered_map<std::string, std::shared_ptr<Shader>> shader_map;
	std::unordered_map<std::string, std::shared_ptr<Texture>> texture_map;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_map;
	std::unordered_map<std::string, std::shared_ptr<Model>> model_map;
	std::unordered_map<std::string, std::shared_ptr<Material>> material_map;
	std::unordered_map<std::string, std::shared_ptr<Font>> font_map;
};
