#include "Engine.h"
#include "FileInput.h"
#include "Node.h"
#include "RendererOpenGL3.h"
#include "RendererVulkan.h"
#include "WindowGLFW.h"
#include "MessagingSystem.h"
#include "CullingSystem.h"
#include "LightingSystem.h"
#include "WindowFactory.h"
#include <iostream>
#include "Texture.h"

namespace B00289996 {
	const std::uint16_t Engine::majorVersion = 0, Engine::minorVersion = 1, Engine::patchVersion = 0;
	RendererType Engine::selectedRenderer = RendererType::OPENGL;
	
	void Engine::ProcessEvent(const Event & toProcess)  {
		if (toProcess.GetTarget() == EventTarget::TARGET_WINDOW_SIZE_OBSERVERS) {
			switch (toProcess.GetType()) {
			case EventType::EVENT_TYPE_UPDATE:
			{
				if (toProcess.GetDataType() == EventDataType::EVENT_DATA_UVEC2) {
					glm::uvec2 size = std::any_cast<glm::uvec2>(toProcess.GetData());
					windowInfo.width = size.x;
					windowInfo.height = size.y;
				}
				if (toProcess.GetDataType() == EventDataType::EVENT_DATA_VEC2) {
					glm::vec2 size = std::any_cast<glm::vec2>(toProcess.GetData());
					windowInfo.positionX = (std::uint32_t)size.x;
					windowInfo.positionY = (std::uint32_t)size.y;
				}
			}
			break;
			case EventType::EVENT_TYPE_NULL:
				std::cout << "Null Event Type Sent To Engine" << std::endl;
				break;
			default:
				std::cout << "The Engine couldn't process an event!" << std::endl;
				break;
			}
		}
		else if (toProcess.GetTarget() == EventTarget::TARGET_SYSTEM_SHUTDOWN_OBSERVERS) {
			switch (toProcess.GetType()) {
			case EventType::EVENT_TYPE_NULL: running = false;
			break;
			default:
				std::cout << "The Engine couldn't process an event!" << std::endl;
				break;
			}
		}
	}

	Engine::Engine() : ConnectionMananger(), nodes(std::vector<std::shared_ptr<Node>>()), filelLoader(std::make_unique<FileInput>()){
		AddConnection(MessagingSystem::GetInstance().AttachObserver(std::bind(&Engine::ProcessEvent, this, std::placeholders::_1), EventTarget::TARGET_WINDOW_SIZE_OBSERVERS));
		AddConnection(MessagingSystem::GetInstance().AttachObserver(std::bind(&Engine::ProcessEvent, this, std::placeholders::_1), EventTarget::TARGET_SYSTEM_SHUTDOWN_OBSERVERS));
		blank = filelLoader->LoadTexture("Defaults", "blank.png");
	}

	Engine::~Engine() {
	}

	void Engine::Init(const RendererType & rendererType, const std::uint32_t & width, const std::uint32_t & height, const WindowType & windowType) {
		selectedRenderer = rendererType;
		updater = std::make_shared<Emitter<const float>>("Updater");
		windowFactory = std::make_unique<WindowFactory>();
		windowInfo.rendererType = rendererType;
		windowInfo.windowType = windowType;
		windowInfo.title = rendererType == RendererType::OPENGL ? "OpenGL Renderer" : "Vulkan Renderer";
		windowInfo.width = width;
		windowInfo.height = height;
		window = windowFactory->CreateUnique(windowInfo);
		window->CreateDisplay();
		if (rendererType == RendererType::OPENGL) renderer = std::make_unique<RendererOpenGL3>();
		else renderer = std::make_unique<RendererVulkan>();
		cullingSystem = std::make_unique<CullingSystem>();
		lightingSystem = std::make_unique<LightingSystem>();
	}

	void Engine::Run(const float & deltaTime) {
		MessagingSystem::GetInstance().ProcessQueue();
		updater->Emit(std::forward<const float>(deltaTime));
		std::vector<CullingResult> results = cullingSystem->GetResults();
		std::vector<std::shared_ptr<Node>> lights = lightingSystem->GetAllLights();
		if (renderer) {
			MessagingSystem::GetInstance().ProcessQueue(EventTarget::TARGET_RENDERING_SYSTEM);
			renderer->BeginRendering();
			std::uint32_t passes = 0;
			for (std::vector<CullingResult>::iterator i = results.begin(); i != results.end(); ++i) {
				std::vector<std::shared_ptr<Node>> toRender = std::vector<std::shared_ptr<Node>>();
				toRender.reserve((*i).visibleNodes.size() + 1);
				toRender.push_back((*i).camera);
				toRender.insert(toRender.end(), lights.begin(), lights.end());
				toRender.insert(toRender.end(), (*i).visibleNodes.begin(), (*i).visibleNodes.end());
				if (toRender.size() > 0) passes++;
				renderer->Render(toRender);
				//std::cout << (*i).visibleNodes.size() << " Objects and " << lights.size() << " lights rendered by camera " << (*i).camera->GetID() << std::endl;
			}
			if(passes == 0) renderer->Render(std::vector<std::shared_ptr<Node>>());
			renderer->EndRendering();
		}
	}

	std::shared_ptr<Node> Engine::CreateNode(const std::string nodeName) {
		std::shared_ptr<Node> toReturn = std::make_shared<Node>(nodeName);
		nodes.push_back(toReturn);
		return toReturn;
	}

	void Engine::SwitchRenderer() {
		renderer = std::unique_ptr<Renderer>();
		window = std::unique_ptr<Window>();
		windowInfo.rendererType = Engine::selectedRenderer = windowInfo.rendererType == RendererType::OPENGL ? RendererType::VULKAN : RendererType::OPENGL;
		windowInfo.title = Engine::selectedRenderer == RendererType::OPENGL ? "OpenGL Renderer" : "Vulkan Renderer";
		window = windowFactory->CreateUnique(windowInfo);
		window->CreateDisplay();
		if (Engine::selectedRenderer == RendererType::OPENGL) renderer = std::make_unique<RendererOpenGL3>();
		else renderer = std::make_unique<RendererVulkan>();
	}

	bool Engine::GetKeyDown(const KeyboardKey & key) {
		return input.GetKeyDown(key);
	}

	bool Engine::GetKey(const KeyboardKey & key) {
		return input.GetKey(key);
	}

	bool Engine::GetKeyUp(const KeyboardKey & key) {
		return input.GetKeyUp(key);
	}

	bool Engine::GetMouseButtonDown(const MouseButton & button) {
		return input.GetMouseButtonDown(button);
	}

	bool Engine::GetMouseButton(const MouseButton & button) {
		return input.GetMouseButton(button);
	}

	bool Engine::GetMouseButtonUp(const MouseButton & button) {
		return input.GetMouseButtonUp(button);
	}

	glm::vec2 Engine::GetMousePosition() {
		return input.GetMousePosition();
	}

	const std::uint32_t Engine::GetWindowWidth() const {
		return windowInfo.width;
	}

	const std::uint32_t Engine::GetWindowHeight() const {
		return windowInfo.height;
	}

	std::shared_ptr<Node> Engine::LoadModel(const std::string & directory, const std::string & fileName, const bool & singleInstance, const bool & flipWinding) {
		return filelLoader->LoadModel(directory, fileName, singleInstance, flipWinding);
	}

	std::shared_ptr<Node> Engine::LoadAnimatedModel(const std::string & directory, const std::string & fileName, const bool & flipWinding) {
		return filelLoader->LoadAnimatedModel(directory, fileName, flipWinding);
	}

	std::shared_ptr<Texture> Engine::LoadTexture(const std::string & directory, const std::string & fileName, const TextureType & type) {
		return filelLoader->LoadTexture(directory, fileName, type);
	}

	void Engine::SetTextureLoader(std::unique_ptr<TextureLoader> & loader) {
		return filelLoader->SetTextureLoader(loader);
	}

	std::shared_ptr<ShaderProgram> Engine::LoadShaderProgram(const std::string & name) {
		return std::shared_ptr<ShaderProgram>();
	}
	std::shared_ptr<Connection> Engine::RegisterWithUpdateCycle(const std::function<void(const float)>& function) {
		return updater->Connect(function);
	}
	void Engine::ToggleCulling() {
		if (cullingSystem) cullingSystem->performCulling = !cullingSystem->performCulling;
	}
}
