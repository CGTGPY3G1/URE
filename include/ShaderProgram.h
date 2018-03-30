#ifndef UNNAMED_SHADER_PROGRAM_H
#define UNNAMED_SHADER_PROGRAM_H
#include "RenderableResource.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <any>

namespace B00289996 {
#define VERTEX_POSITION		0
#define VERTEX_TEXCOORD		1
#define VERTEX_NORMAL		2
#define VERTEX_TANGENT		3
#define VERTEX_BITANGENT	4
#define VERTEX_COLOUR		5
#define VERTEX_BONE_IDS		6
#define VERTEX_BONE_WEIGHTS	7
	template<typename T>
	struct is_vector {
		static constexpr bool value = false;
	};

	template<template<typename...> class C, typename U>
	struct is_vector<C<U>> {
		static constexpr bool value =
			std::is_same<C<U>, std::vector<U>>::value;
	};

	enum ShaderDataType {
		ATTRIBUTE_DATA,
		UNIFORM_DATA
	};
	enum ShaderDataFormat { NULL_FORMAT, BOOL, INT, FLOAT, VEC2, VEC3, VEC4, MAT3, MAT4, IMAGE1D, IMAGE2D, IMAGE3D, POINTLIGHT, MATERIAL };
	enum ShaderStage { VERTEX, FRAGMENT };
	struct AttributeValue {
		AttributeValue(const std::string & attributeName, const std::uint32_t & attributeLocation) : name(attributeName), location(attributeLocation) {}
		std::string name;
		std::uint32_t location;
	};
	struct UniformValue {
		UniformValue(const std::string & uniformName, const ShaderDataFormat & uniformFormat, std::vector<ShaderStage> usedInStages, const std::uint32_t & max = 1)
			: name(uniformName), dataFormat(uniformFormat), stages(usedInStages), numberOfOccurances(max) {}
		std::string name;
		ShaderDataFormat dataFormat;
		const std::uint32_t numberOfOccurances;
		std::vector<ShaderStage> stages;
	};

	const static std::vector<AttributeValue> RESERVED_ATTRIBUTES = {
		AttributeValue("Position", 0),
		AttributeValue("UV1", 1),
		AttributeValue("Normal", 2),
		AttributeValue("Tangent", 3),
		AttributeValue("BiTangent", 4),
		AttributeValue("Colour", 5)
	};
	class ShaderDataContainer {
		struct DataType {
			using Container = std::unique_ptr<DataType>;
			virtual const std::any Get() const = 0;
		};
		template <typename T> struct ConcreteDataType : DataType {
			ConcreteDataType(T newData) : data(newData) {}
			const std::any Get() const override { return data; }
		private:
			T data;
		};
	public:		
		template <typename T> ShaderDataContainer(T src, const std::string & name, const ShaderDataType & dataType, const ShaderDataFormat & dataFormat)
			: data(new ConcreteDataType<T>(std::forward<T>(src))), mName(name), type(dataType), format(dataFormat), isArray(is_vector<T>::value) {}
		~ShaderDataContainer() {}
		const std::any GetData() const { return data->Get(); }
	private:
		typename DataType::Container data;
		std::string mName;
		ShaderDataType type;
		ShaderDataFormat format;
		bool isArray;
	};

	struct FindUniformByName {
		FindUniformByName(const std::string & name) : n(name) {}
		const bool operator()(const UniformValue & other) const { return other.name.compare(n) == 0; }
	private:
		std::string n;
	};
	struct FindUniformByFormat {
		FindUniformByFormat(const ShaderDataFormat & format) : f(format) {}
		const bool operator()(const UniformValue & other) const { return other.dataFormat == f; }
	private:
		ShaderDataFormat f;
	};
	struct FindAttributeByName {
		FindAttributeByName(const std::string & name) : n(name) {}
		const bool operator()(const AttributeValue & other) const { return other.name.compare(n) == 0; }
	private:
		std::string n;
	};


	enum ShaderType : std::uint8_t {
		VERTEX_SHADER,
		FRAGMENT_SHADER
	};
	
	static std::string ShaderTypeChar(const ShaderType & type) {
		switch (type) {
		case ShaderType::FRAGMENT_SHADER: return "frag"; break;
		case ShaderType::VERTEX_SHADER: return "vert"; break;
		default:
			break;
		}
	}

	struct ShaderValue {
		std::string name;
		std::size_t type;
	};

	struct Shader {
		Shader();
		Shader(const ShaderType & shaderType, const std::string & shaderName = "Default");

		ShaderType type;
		std::string name, fileType;
		const bool operator ==(const Shader & other) const;
	};
	class ShaderProgram : RenderableResource {
		friend class FileInput;
		friend struct std::hash<ShaderProgram>;
		friend struct std::hash<std::shared_ptr<ShaderProgram>>;
	public:
		
		/// <summary>Initializes a new instance of the <see cref="ShaderProgram"/> class.</summary>
		ShaderProgram(const std::string & shaderName = "Unnamed");
		/// <summary>Finalizes an instance of the <see cref="ShaderProgram"/> class.</summary>
		~ShaderProgram();
		void AddShader(const Shader & shader);
		void AddUniform(const UniformValue & uniform);
		const std::vector<UniformValue> GetUniforms() const;
		const bool HasUniform(const std::string & uniformName)const;
		void AddAttribute(const AttributeValue & attribute);
		const std::vector<AttributeValue> GetAttributes() const;
		const bool HasAttribute(const std::string & attributeName)const;
		const bool operator ==(const std::shared_ptr<ShaderProgram> & other) const;
		const bool operator !=(const std::shared_ptr<ShaderProgram> & other) const;
		const std::uint16_t GetID() const;
		const std::vector<Shader> GetShaders() const;
		const std::string GetName() const;
	private:
		std::vector<Shader> shaders;
		std::vector<AttributeValue> attributes;
		std::vector<UniformValue> uniforms;
		bool initialized, dirty;
		std::uint16_t id;
		std::string name;
	};
}

namespace std {
	template <>
	struct hash<B00289996::ShaderProgram> {
		std::size_t operator()(const B00289996::ShaderProgram& k) const {
			return k.id;
		}
	};

	template <>
	struct hash<std::shared_ptr<B00289996::ShaderProgram>> {
		std::size_t operator()(const std::shared_ptr<B00289996::ShaderProgram>& k) const {
			return k->id;
		}
	};
}

#endif // !UNNAMED_SHADER_PROGRAM_H
