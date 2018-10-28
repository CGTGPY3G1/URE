#include "Material.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
namespace B00289996 {
	const float Material::min = 0.0f, Material::max = 1.0f;
	Material::Material() : ambient(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), diffuse(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)), specular(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)), shininess(0.8f),
	textures(std::vector<std::shared_ptr<Texture>>()), shader(std::shared_ptr<ShaderProgram>()) {
	}

	Material::~Material() {
	}

	void Material::ReplaceTexture(const std::size_t & index, const std::shared_ptr<Texture>& newTexture, const bool & addIfNotFound) {
		if (index < textures.size()) textures[index] = newTexture;
		else if (addIfNotFound) textures.push_back(newTexture);
	}

	void Material::AddTexture(const std::shared_ptr<Texture>& toAdd) {
		if (toAdd) textures.push_back(toAdd);
	}

	void Material::AddTextures(const std::vector<std::shared_ptr<Texture>>& toAdd) {
		for (size_t i = 0; i < toAdd.size(); i++) {
			if (toAdd[i]) textures.push_back(toAdd[i]);
		}
	}

	const std::shared_ptr<Texture> Material::GetTexture(const std::size_t & index) const {
		return index < textures.size() ? textures[index] : std::shared_ptr<Texture>();
	}

	const std::vector<std::shared_ptr<Texture>>& Material::GetTextures() const {
		return textures;
	}

	void Material::RemoveTextures() {
		textures.clear();
	}

	const std::size_t Material::GetNumberOfTextures() const {
		return textures.size();
	}

	const glm::vec4 Material::GetAmbient() const {
		return ambient;
	}

	void Material::SetAmbient(const glm::vec4 & newValue) {
		ambient = glm::clamp(newValue, min, max);
	}

	const glm::vec4 Material::GetDiffuse() const {
		return diffuse;
	}

	void Material::SetDiffuse(const glm::vec4 & newValue) {
		diffuse = glm::clamp(newValue, min, max);
	}

	const glm::vec4 Material::GetSpecular() const {
		return specular;
	}

	void Material::SetSpecular(const glm::vec4 & newValue) {
		specular = glm::clamp(newValue, min, max);
	}

	const float Material::GetShininess() const {
		return shininess;
	}

	void Material::SetShininess(const float & newValue) {
		shininess = glm::clamp(newValue, min, max);
	}
	const std::shared_ptr<ShaderProgram> Material::GetShader() const {
		return shader;
	}
	void Material::SetShader(const std::shared_ptr<ShaderProgram>& shader) {
		if(shader) this->shader = shader;
	}
}
