#ifndef UNNAMED_CONTEXT_MANAGER_H
#define UNNAMED_CONTEXT_MANAGER_H
#include <unordered_map>
#include <memory>
#define VERTEX_POSITION		0
#define VERTEX_TEXCOORD		1
#define VERTEX_NORMAL		2
#define VERTEX_TANGENT		3
#define VERTEX_BITANGENT	4
#define VERTEX_COLOUR		5
#define VERTEX_MAX_BUFFERS	6
namespace B00289996 {

	class Mesh;
	class Texture;
	class ShaderProgram;
	class ContextManager {
	public:
		ContextManager();
		virtual ~ContextManager();
		virtual void RegisterTexture(const std::shared_ptr<Texture>& texture) = 0;
		virtual void RegisterMesh(const std::shared_ptr<Mesh>& mesh) = 0;
		virtual void RegisterShader(const std::shared_ptr<ShaderProgram>& shader) = 0;
		virtual void UnregisterTexture(const std::shared_ptr<Texture>& texture) = 0;
		virtual void UnregisterMesh(const std::shared_ptr<Mesh>& mesh) = 0;
		virtual void UnregisterShader(const std::shared_ptr<ShaderProgram>& shader) = 0;
	private:
	};
}

#endif // !UNNAMED_CONTEXT_MANAGER_H