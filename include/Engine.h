#ifndef UNNAMED_ENGINE_H
#define UNNAMED_ENGINE_H
#include <memory>
#include <vector>
#include <unordered_map>
#include "Emitter.h"
#include "Enums.h"
#include "ObjectCreationInfo.h"
#include "Input.h"
#include "Texture.h"
#include <functional>


namespace B00289996 {
	constexpr uint32_t MAX_LIGHTS = 10, MAX_TEXTURES = 50;// , MAX_BONE_WEIGHTS = 4, MAX_BONES = 128;
	class Node;
	class WindowFactory;
	class Renderer;
	class Window;
	class CullingSystem;
	class LightingSystem;
	class ShaderProgram;
	class TextureLoader;
	class FileInput;
	class Engine : public ConnectionMananger {
	public:
		const static std::uint16_t majorVersion, minorVersion, patchVersion;
		static RendererType selectedRenderer;
		static Engine & GetInstance() {
			static Engine instance;
			return instance;
		}
		~Engine();
		void Init(const RendererType & rendererType, const std::uint32_t & width = 1280, const std::uint32_t & height = 720, const WindowType & windowType = WindowType::WINDOW_TYPE_GLFW);
		void Run(const float & deltaTime);
		std::shared_ptr<Node> CreateNode(const std::string nodeName = "UnnamedNode");
		void SwitchRenderer();
		/// <summary>Returns true if the key was pressed during the last frame.</summary>
		/// <param name="key">The KeyboardKey.</param>
		/// <returns>true if the key was pressed during the last frame, else false</returns>
		bool GetKeyDown(const KeyboardKey & key);
		/// <summary>Returns true if the key was held down during the last frame.</summary>
		/// <param name="key">The KeyboardKey.</param>
		/// <returns>true if the key was held down during the last frame, else false</returns>
		bool GetKey(const KeyboardKey & key);
		/// <summary> Returns true if the key was released during the last frame. </summary>
		/// <param name="key">The KeyboardKey.</param>
		/// <returns>true if the key was released during the last frame, else false</returns>
		bool GetKeyUp(const KeyboardKey & key);
		/// <summary>Returns true if the mouse button was pressed during the last frame.</summary>
		/// <param name="button">The button.</param>
		/// <returns>true if the mouse button was pressed during the last frame, else false</returns>
		bool GetMouseButtonDown(const MouseButton & button);
		/// <summary> Returns true if the mouse button was held down during the last frame.</summary>
		/// <param name="button">The button.</param>
		/// <returns>true if the mouse button was held down during the last frame, else false</returns>
		bool GetMouseButton(const MouseButton & button);
		/// <summary>Returns true if the mouse button was released during the last frame.</summary>
		/// <param name="button">The button.</param>
		/// <returns>true if the mouse button was released during the last frame, else false</returns>
		bool GetMouseButtonUp(const MouseButton & button);
		/// <summary> Gets the mouse position.</summary>
		/// <returns>the mouse position</returns>
		glm::vec2 GetMousePosition();
		const std::uint32_t GetWindowWidth() const;
		const std::uint32_t GetWindowHeight() const;
		const bool IsRunning() const { return running; }
		std::shared_ptr<Node> LoadModel(const std::string & directory, const std::string & fileName, const bool & singleInstance = true, const bool & flipWinding = false);
		std::shared_ptr<Node> LoadAnimatedModel(const std::string & directory, const std::string & fileName, const bool & flipWinding = false);
		std::shared_ptr<Texture> LoadTexture(const std::string & directory, const std::string & fileName, const TextureType & type = TextureType::DIFFUSE);
		void SetTextureLoader(std::unique_ptr<TextureLoader> & loader);
		std::shared_ptr<ShaderProgram> LoadShaderProgram(const std::string & name);
		std::shared_ptr<Connection> RegisterWithUpdateCycle(const std::function<void(const float)> & function);
		void ToggleCulling();
	private:
		void ProcessEvent(const Event & toProcess);
		Engine();
		WindowCreationInfo windowInfo;
		std::vector<std::shared_ptr<Node>> nodes;
		std::shared_ptr<Emitter<const float>> updater;
		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<Window> window;
		std::unique_ptr<CullingSystem> cullingSystem;
		std::unique_ptr<LightingSystem> lightingSystem;
		std::unique_ptr<WindowFactory> windowFactory;
		std::unique_ptr<FileInput> filelLoader;
		std::shared_ptr<Texture> blank;
		Input input;
		bool running = true;
	};
}
#endif // !UNNAMED_ENGINE_H