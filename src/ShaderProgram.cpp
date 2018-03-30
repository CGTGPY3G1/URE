#include "ShaderProgram.h"
#include "IDGenerator.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <algorithm>
#include <iostream>
namespace B00289996 {
	ShaderProgram::ShaderProgram(const std::string & shaderName): name(shaderName), shaders(std::vector<Shader>()), uniforms(std::vector<UniformValue>()), attributes(std::vector<AttributeValue>()) {
		id = IDGenerator<ShaderProgram, std::uint16_t>::GetInstance().GetNewID();
	}
	ShaderProgram::~ShaderProgram() {
		IDGenerator<ShaderProgram, std::uint16_t>::GetInstance().RetireID(id);
	}
	void ShaderProgram::AddShader(const Shader & shader) {
		shaders.push_back(shader);
	}

	void ShaderProgram::AddUniform(const UniformValue & uniform) {
		if (uniform.dataFormat == ShaderDataFormat::NULL_FORMAT) std::cout << "Attempted to add NULL data format uniform to shader" << std::endl;
		else {
			std::vector<UniformValue>::iterator i = std::find_if(uniforms.begin(), uniforms.end(), FindUniformByName(uniform.name));
			if (i == uniforms.end()) {
				uniforms.push_back(uniform);
			}
		}
	}

	const std::vector<UniformValue> ShaderProgram::GetUniforms() const {
		return uniforms;
	}

	const bool ShaderProgram::HasUniform(const std::string & uniformName) const {
		std::vector<UniformValue>::const_iterator i = std::find_if(uniforms.begin(), uniforms.end(), FindUniformByName(uniformName));
		return i != uniforms.end();
	}

	void ShaderProgram::AddAttribute(const AttributeValue & attribute) {
		std::vector<AttributeValue>::iterator i = std::find_if(attributes.begin(), attributes.end(), FindAttributeByName(attribute.name));
		if (i == attributes.end()) {
			attributes.push_back(attribute);
		}
		else {
			std::cout << "Attribute " << attribute.name << " already added" << std::endl;
		}
	}

	const std::vector<AttributeValue> ShaderProgram::GetAttributes() const {
		return attributes;
	}

	const bool ShaderProgram::HasAttribute(const std::string & attributeName) const {
		std::vector<AttributeValue>::const_iterator i = std::find_if(attributes.begin(), attributes.end(), FindAttributeByName(attributeName));
		return i != attributes.end();
	}

	const bool ShaderProgram::operator==(const std::shared_ptr<ShaderProgram>& other) const {
		return other && id == other->id;
	}
	const bool ShaderProgram::operator!=(const std::shared_ptr<ShaderProgram>& other) const {
		return !(*this == other);
	}
	const std::uint16_t ShaderProgram::GetID() const {
		return id;
	}
	const std::vector<Shader> ShaderProgram::GetShaders() const {
		return shaders;
	}
	const std::string ShaderProgram::GetName() const {
		return name;
	}
	Shader::Shader() {
	}
	Shader::Shader(const ShaderType & shaderType, const std::string & shaderName) : type(shaderType), name(shaderName) {

	}
	const bool Shader::operator==(const Shader & other) const {
		return type == other.type && name == other.name && fileType == other.fileType;
	}
}
