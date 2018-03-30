#ifndef UNNAMED_CONTEXT_MANAGER_OPENGL3_H
#define UNNAMED_CONTEXT_MANAGER_OPENGL3_H
#include "ContextManager.h"
#include <unordered_map>
#include <memory>
#include <GL/glew.h>

namespace B00289996 {
	class ContextManagerOpenGL3 : public ContextManager {
	public:
		ContextManagerOpenGL3();
		~ContextManagerOpenGL3();
		GLuint GetTextureID(const std::shared_ptr<Texture> & texture);
		GLuint GetMeshID(const std::shared_ptr<Mesh>& mesh);
		GLuint GetShaderID(const std::shared_ptr<ShaderProgram>& shader);
		void RegisterTexture(const std::shared_ptr<Texture>& texture) override;
		void RegisterMesh(const std::shared_ptr<Mesh>& mesh) override;
		void RegisterShader(const std::shared_ptr<ShaderProgram>& shader) override;
		void UnregisterTexture(const std::shared_ptr<Texture>& texture) override;
		void UnregisterMesh(const std::shared_ptr<Mesh>& mesh) override;
		void UnregisterShader(const std::shared_ptr<ShaderProgram>& shader) override;
	private:
		GLuint LoadTexture(const std::shared_ptr<Texture>& texture);
		GLuint LoadMesh(const std::shared_ptr<Mesh>& mesh);
		GLuint LoadShader(const std::shared_ptr<ShaderProgram>& shader);
		std::unordered_map<std::uint16_t, GLuint> meshes;
		std::unordered_map<std::uint16_t, std::pair<GLuint, GLuint>> meshBuffers;
		std::unordered_map<std::uint16_t, GLuint> shaderPrograms;
		std::unordered_map<std::uint16_t, GLuint> textures;
	};
}

#endif // !UNNAMED_CONTEXT_MANAGER_OPENGL3_H
