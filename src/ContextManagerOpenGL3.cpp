#include "ContextManagerOpenGL3.h"
#include "Texture.h"
#include "Geometry.h"
#include "ShaderProgram.h"
#include <iostream>
#include "TextParser.h"
#include "Debug.h"
namespace B00289996 {
	ContextManagerOpenGL3::ContextManagerOpenGL3() : meshes(std::unordered_map<std::uint16_t, GLuint>()),
		shaderPrograms(std::unordered_map<std::uint16_t, GLuint>()), textures(std::unordered_map<std::uint16_t, GLuint>()) {
	}
	ContextManagerOpenGL3::~ContextManagerOpenGL3() {
		for (std::unordered_map<std::uint16_t, GLuint>::iterator i = meshes.begin(); i != meshes.end(); ++i) {
			GLuint id = (*i).second;
			glDeleteBuffers(1, &meshBuffers[id].first);
			glDeleteBuffers(1, &meshBuffers[id].second);
			glDeleteVertexArrays(1, &id);

		}
		for (std::unordered_map<std::uint16_t, GLuint>::iterator i = textures.begin(); i != textures.end(); ++i) {
			glDeleteTextures(1, &(*i).second);
		}
		for (std::unordered_map<std::uint16_t, GLuint>::iterator i = shaderPrograms.begin(); i != shaderPrograms.end(); ++i) {
			glDeleteProgram((*i).second);
		}
	}

	GLuint ContextManagerOpenGL3::GetTextureID(const std::shared_ptr<Texture>& texture) {
		const std::uint16_t id = texture->GetID();
		if (textures.count(id) > 0) return textures[id];
		return LoadTexture(texture);
	}

	GLuint ContextManagerOpenGL3::GetMeshID(const std::shared_ptr<Mesh>& mesh) {
		const std::uint16_t id = mesh->GetID();
		if (meshes.count(id) > 0) return meshes[id];
		return LoadMesh(mesh);
	}

	GLuint ContextManagerOpenGL3::GetShaderID(const std::shared_ptr<ShaderProgram>& shader) {
		const std::uint16_t id = shader->GetID();
		if (shaderPrograms.count(id) > 0) return shaderPrograms[id];
		return LoadShader(shader);
	}

	void ContextManagerOpenGL3::RegisterTexture(const std::shared_ptr<Texture>& texture) {
		if (texture) {
			const std::uint16_t id = texture->GetID();
			if (textures.count(id) > 0) std::cout << "Texture " << id << " already registered!" << std::endl;
			else LoadTexture(texture);
		}
	}

	void ContextManagerOpenGL3::RegisterMesh(const std::shared_ptr<Mesh>& mesh) {
		if (mesh) {
			const std::uint16_t id = mesh->GetID();
			if (meshes.count(id) > 0) std::cout << "Mesh " << id << " already registered!" << std::endl;
			else LoadMesh(mesh);
		}
	}

	void ContextManagerOpenGL3::RegisterShader(const std::shared_ptr<ShaderProgram>& shader) {
		if (shader) {
			const std::uint16_t id = shader->GetID();
			if (shaderPrograms.count(id) > 0) std::cout << "Shader Program " << id << " already registered!" << std::endl;
			else LoadShader(shader);
		}
	}

	void ContextManagerOpenGL3::UnregisterTexture(const std::shared_ptr<Texture>& texture) {
		if (texture) {
			const std::uint16_t id = texture->GetID();
			if (textures.count(id) > 0) {
				std::cout << "Texture " << id << " unregistered" << std::endl;
				textures.erase(id);
			}
			else std::cout << "Couldn't unregister Texture " << id << std::endl;
		}
	}

	void ContextManagerOpenGL3::UnregisterMesh(const std::shared_ptr<Mesh>& mesh) {
		if (mesh) {
			const std::uint16_t id = mesh->GetID();
			if (meshes.count(id) > 0) {
				GLuint VAO = meshes[id];
				std::pair<GLuint, GLuint> buffers = meshBuffers[VAO];
				glDeleteVertexArrays(1, &meshes[id]);
				glDeleteBuffers(1, &buffers.first);
				glDeleteBuffers(1, &buffers.second);
				meshes.erase(id);
				meshBuffers.erase(VAO);
				std::cout << "Mesh " << id << " Unregistered " << VAO << std::endl;
			}
			else std::cout << "Couldn't unregister Mesh " << id << std::endl;
		}
	}

	void ContextManagerOpenGL3::UnregisterShader(const std::shared_ptr<ShaderProgram>& shader) {
		if (shader) {
			const std::uint16_t id = shader->GetID();
			if (shaderPrograms.count(id) > 0) {
				shaderPrograms.erase(id);
				std::cout << "Shader " << id << " Unregistered" << std::endl;
			}
			else std::cout << "Couldn't unregister Shader Program " << id << std::endl;
		}
	}

	GLuint ContextManagerOpenGL3::LoadTexture(const std::shared_ptr<Texture>& texture) {
		if (!texture) {
			std::cout << "Null Texture sent to ContextManager" << std::endl;
			return (GLuint)0;
		}
		const std::uint16_t id = texture->GetID();
		if (textures.count(id) != 0) return textures[id];
		else {
			unsigned char * imageData = texture->GetTextureData();
			if (imageData == nullptr) {
				std::cout << "Texture " << id << " is invalid" << std::endl;
				return(GLuint)0;
			}
			else {
				GLuint textureID = 0;
				// activate the relevant texture type
				glActiveTexture(GL_TEXTURE0 + (std::uint32_t)texture->GetType());
				// create a new texture
				glGenTextures(1, &textureID);
				glBindTexture(GL_TEXTURE_2D, textureID);
				// populate the generated texure with the image data
				GLuint64 channels = texture->GetChannels() == 4 ? GL_RGBA : GL_RGB;
				glTexImage2D(GL_TEXTURE_2D, 0, channels, (GLsizei)texture->GetWidth(), (GLsizei)texture->GetHeight(), 0, channels, GL_UNSIGNED_BYTE, imageData);
				// set up texture parameter
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glBindTexture(GL_TEXTURE_2D, 0);
				if (id != 0) {
					textures[id] = textureID; // store the texture for re-use
					std::cout << "Texture " << id << " Loaded" << std::endl;
				}
				else std::cout << "Texture " << id << " is Invalid" << std::endl;
				return textureID; // return the texture id
			}
		}
		return (GLuint)0;
	}
	GLuint ContextManagerOpenGL3::LoadMesh(const std::shared_ptr<Mesh>& mesh) {
		GLuint toReturn = 0;
		if (mesh) {
			std::vector<Vertex> vertices = mesh->GetVertices();
			std::vector<std::uint32_t> indices = mesh->GetIndices();
			if (!vertices.empty() && !indices.empty()) {
				GLuint VAO = 0;
				GLuint VBO = 0;
				GLuint IBO = 0;
				// Generate VBO and VAOs
				glGenVertexArrays(1, &VAO);
				glBindVertexArray(VAO);
				// use a single buffer to store all vertex data
				glGenBuffers(1, &VBO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				//load the vertex spectification data int the buffer
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

				// define the offsets for each buffer
				glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
				glEnableVertexAttribArray(VERTEX_POSITION);
				glVertexAttribPointer(VERTEX_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
				glEnableVertexAttribArray(VERTEX_TEXCOORD);
				glVertexAttribPointer(VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
				glEnableVertexAttribArray(VERTEX_NORMAL);
				glVertexAttribPointer(VERTEX_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
				glEnableVertexAttribArray(VERTEX_TANGENT);
				glVertexAttribPointer(VERTEX_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));
				glEnableVertexAttribArray(VERTEX_BITANGENT);
				glVertexAttribPointer(VERTEX_COLOUR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
				glEnableVertexAttribArray(VERTEX_COLOUR);
				glVertexAttribIPointer(VERTEX_BONE_IDS, 4, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
				glEnableVertexAttribArray(VERTEX_BONE_IDS);
				glVertexAttribPointer(VERTEX_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));
				glEnableVertexAttribArray(VERTEX_BONE_WEIGHTS);

				// generate and populate the index buffer
				glGenBuffers(1, &IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				if (VAO != 0) {
					meshBuffers[VAO] = std::pair<GLuint, GLuint>(VBO, IBO);
					meshes[mesh->GetID()] = VAO;
					toReturn = VAO;
				}
				std::cout << "Mesh " << mesh->GetID() << " Loaded into VAO " << VAO << std::endl;
			}
			else std::cout << "Mesh " << mesh->GetID() << " is invalid " << std::endl;
		}

		return toReturn;
	}

	// print shader errors
	void PrintShaderError(const GLuint & id, const std::string & prefix = "") {
		GLint logLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
		GLchar * errorLog = new GLchar[logLength];
		int l = 0;
		glGetShaderInfoLog(id, logLength, &l, errorLog);
		Debug::PrintImmediately(prefix + errorLog, DebugMessageType::DEBUG_TYPE_FAILURE_CRITICAL);
	}
	// print shader program errors
	void PrintShaderProgramError(const GLuint & id, const std::string & prefix = "") {
		GLint logLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
		GLchar * errorLog = new GLchar[logLength];
		int l = 0;
		glGetProgramInfoLog(id, logLength, &l, errorLog);
		Debug::PrintImmediately(prefix + errorLog, DebugMessageType::DEBUG_TYPE_FAILURE_CRITICAL);
	}

	GLuint ContextManagerOpenGL3::LoadShader(const std::shared_ptr<ShaderProgram>& shader) {
		GLuint toReturn = 0;
		if (shader) {
			const std::uint16_t id = shader->GetID();
			if (shaderPrograms.count(id) != 0) return shaderPrograms[id]; // if the shader program has already been loaded, return it
			else {
				struct TempValue {
					TempValue(const Shader & s, const std::string & src) : shader(s), source(src) {}
					Shader shader;
					std::string source;
				};

				std::vector<GLuint> shaderIDs;
				std::vector<Shader> shaders = shader->GetShaders();
				for (size_t i = 0; i < shaders.size(); i++) {
					Shader s = shaders[i];
					std::string location = "../assets/Shaders/OpenGL3/" + s.name + "." + (s.type == ShaderType::VERTEX_SHADER ? "vert" : "frag");
					std::string source = TextParser::ReadFile(location);
					if (source.empty()) {
						Debug::PrintImmediately("Invalid Shader Source - " + location, DebugMessageType::DEBUG_TYPE_FAILURE_CRITICAL);
						return (GLuint)0;
					}
					else {

						GLuint sID = glCreateShader(s.type == ShaderType::VERTEX_SHADER ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
						// compile the shader sources
						const char * cSource = source.c_str();
						GLint cSize = (GLint)source.size();
						glShaderSource(sID, 1, &cSource, &cSize);
						glCompileShader(sID);
						// check the status of the vertex shader
						GLint status;
						glGetShaderiv(sID, GL_COMPILE_STATUS, &status);
						if (status == GL_FALSE) {
							PrintShaderError(sID, "Couldn't compile shader source :");
							return (GLuint)0;
						}
						else {
							shaderIDs.push_back(sID);
						}
					}
				}

				GLuint shaderProgram = glCreateProgram();
				for (size_t i = 0; i < shaderIDs.size(); i++) {
					glAttachShader(shaderProgram, shaderIDs[i]);
				}
				// bind the default attribute locations
				glBindAttribLocation(shaderProgram, VERTEX_POSITION, "Position");
				glBindAttribLocation(shaderProgram, VERTEX_TEXCOORD, "UV1");
				glBindAttribLocation(shaderProgram, VERTEX_NORMAL, "Normal");
				glBindAttribLocation(shaderProgram, VERTEX_TANGENT, "Tangent");
				glBindAttribLocation(shaderProgram, VERTEX_BITANGENT, "Bitangent");
				glBindAttribLocation(shaderProgram, VERTEX_COLOUR, "Colour");
				glBindAttribLocation(shaderProgram, VERTEX_BONE_IDS, "BoneIDs");
				glBindAttribLocation(shaderProgram, VERTEX_BONE_WEIGHTS, "BoneWeights");
				glBindTextureUnit(GL_TEXTURE0, 0);
				// link and bind the shader program
				glLinkProgram(shaderProgram);
				int isLinked = -1;
				glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
				// if the shader failed to link
				if (isLinked == GL_FALSE) {
					PrintShaderError(id, "Couldn't link shader :");
					return (GLuint)0;
				}
				for (std::vector<GLuint>::iterator i = shaderIDs.begin(); i != shaderIDs.end(); ++i) glDetachShader(shaderProgram, (*i));
				glUseProgram(shaderProgram);
				toReturn = shaderProgram;
				shaderPrograms[id] = toReturn;
			}
		}
		return toReturn;
	}
}
