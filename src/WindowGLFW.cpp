#include "WindowGLFW.h"
#include "MessagingSystem.h"
#include "Engine.h"
#include "VulkanInstanceData.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <GLFW/glfw3native.h>

namespace B00289996 {
	WindowGLFW::WindowGLFW(const std::string & name, const std::uint32_t & width, const std::uint32_t & height, const RendererType & rendererType, const std::uint32_t & positionX, const std::uint32_t & positionY)
		: Window(name, width, height, rendererType, positionX, positionY), window(nullptr), title(name){
		initialized = glfwInit();
		if (!initialized) {
			std::cout << "GLFW Initialization failed!" << std::endl;
		}
		this->width = width; this->height = height;
		this->positionX = positionX; this->positionY = positionY;
	}

	WindowGLFW::~WindowGLFW() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	void WindowGLFW::ResizedCallback(GLFWwindow *window, int width, int height) {
		int maxed = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
		if (maxed == GL_TRUE && !Window::maximised) Window::maximised = true;
		if (maxed == GL_FALSE && Window::maximised) Window::maximised = false;
		Event e = Event(EventType::EVENT_TYPE_UPDATE, EventTarget::TARGET_WINDOW_SIZE_OBSERVERS, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_UVEC2, std::any(glm::uvec2(width >= 0 ? width : 1, height >= 0 ? height : 1)));
		MessagingSystem::GetInstance().SendEvent(e);
	}

	void WindowGLFW::RepositionedCallback(GLFWwindow * window, int xpos, int ypos) {
		Event e = Event(EventType::EVENT_TYPE_UPDATE, EventTarget::TARGET_WINDOW_SIZE_OBSERVERS, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_VEC2, std::any(glm::vec2(xpos, ypos)));
		MessagingSystem::GetInstance().SendEvent(e);
	}

	void WindowGLFW::CreateDisplay() {
		if(!initialized) initialized = glfwInit();
		if (initialized) {
			glfwDefaultWindowHints();
			glfwWindowHint(GLFW_MAXIMIZED, Window::maximised);
			if (mRendererType == RendererType::OPENGL) {////////////////////////OpenGL/////////////////////////////
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
				window = glfwCreateWindow(width, height, mName.c_str(), nullptr, nullptr);
				glfwSetWindowPos(window, positionX, positionY);
				if (window == nullptr) {
					std::cout << "GLFW window creation failed!" << std::endl;
					return;
				}
				glfwMakeContextCurrent(window);
				glfwSwapInterval(0);
				glewExperimental = true;
				if (glewInit() != GLEW_OK) {
					std::cout << "glewInit failed." << std::endl;
					return;
				}	
			}
			else if(mRendererType == RendererType::VULKAN) {////////////////////////Vulkan/////////////////////////////
				if (!glfwVulkanSupported()) {
					std::cout << "Vulkan is not supported!" << std::endl;
					return;
				}
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				window = glfwCreateWindow(mWidth, mHeight, mName.c_str(), nullptr, nullptr);
				glfwSetWindowPos(window, positionX, positionY);
				if (window == nullptr) {
					std::cout << "GLFW window creation failed!" << std::endl;
					return;
				}
				std::uint32_t extensionCount = 0; // an output placeholder
				const char ** ex = glfwGetRequiredInstanceExtensions(&extensionCount);
				std::vector<const char*> layers;
				std::vector<const char*> extensions;
				#if _DEBUG
				extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
				layers.push_back("VK_LAYER_LUNARG_standard_validation");
				#endif

				if (extensionCount > 0) {
					std::cout << "Required Vulkan Instance Extensions:" << std::endl;
					for (size_t i = 0; i < extensions.size(); i++) {
						std::cout << extensions[i] << std::endl;
					}
					for (std::uint32_t i = 0; i < extensionCount; ++i) {
						std::cout << ex[i] << std::endl;
						extensions.push_back(ex[i]);
					}
				}
				
				VulkanInstanceData instanceData;
				vk::ApplicationInfo appInfo = vk::ApplicationInfo()
					.setPApplicationName(mName.c_str())
					.setPEngineName(mName.c_str())
					.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
					.setApplicationVersion(1)
					.setApiVersion(VK_MAKE_VERSION(1, 0, 6));
				vk::InstanceCreateInfo instanceInfo({}, &appInfo, (std::uint32_t)layers.size(), layers.data(), (std::uint32_t)extensions.size(), extensions.data());
				vk::Result result = vk::createInstance(&instanceInfo, nullptr, &instanceData.instance);
				if (result != vk::Result::eSuccess) {
					std::cout << "Failed to create vulkan instance!" << std::endl;
				}
				else {
					std::vector<vk::PhysicalDevice> physicalDevices = instanceData.instance.enumeratePhysicalDevices();
					assert(physicalDevices.size() > 0);
					bool foundSuitableDevice = false;
					extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
					for (size_t i = 0; i < physicalDevices.size() && !foundSuitableDevice; i++) {						
						std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevices[i].getQueueFamilyProperties();
						for (size_t j = 0; j < queueFamilyProperties.size() && !foundSuitableDevice; j++) {
							if (queueFamilyProperties[j].queueFlags & vk::QueueFlagBits::eGraphics) {
								instanceData.device = std::make_shared<VulkanDevice>(physicalDevices[i], layers, extensions);
								foundSuitableDevice = true;
							}
						}
					}
					if (!foundSuitableDevice) {
						std::cout << "Couldn't find suitable device!" << std::endl;
					}
					
					VkSurfaceKHR temp{};
					VkResult r = glfwCreateWindowSurface(VkInstance(instanceData.instance), window, nullptr, &temp);
					vk::SurfaceKHR surface(temp);
					if (r != VK_SUCCESS) {
						std::cout << "Failed to create vulkan window!" << std::endl;
					}
					else {
						instanceData.surface = std::make_shared<VulkanSurface>(mWidth, mHeight, surface, instanceData.device);
						std::any data(instanceData);
						Event e = Event(EventType::EVENT_TYPE_REGISTER, EventTarget::TARGET_RENDERING_SYSTEM, EventPriority::EVENT_ADD_QUEUE, EventDataType::EVENT_DATA_VULKAN_INSTANCE, data);
						MessagingSystem::GetInstance().SendEvent(e);
					}
				}
			}
		}

		if (window) {
			glfwSetWindowSizeCallback(window, &WindowGLFW::ResizedCallback);
			glfwSetWindowPosCallback(window, &WindowGLFW::RepositionedCallback);
		}
	}

	std::int32_t WindowGLFW::ToGLFWKey(const KeyboardKey & key) {
		switch (key) {
		case KeyboardKey::KEY_0: return GLFW_KEY_0;
		case KeyboardKey::KEY_1: return GLFW_KEY_1;
		case KeyboardKey::KEY_2: return GLFW_KEY_2;
		case KeyboardKey::KEY_3: return GLFW_KEY_3;
		case KeyboardKey::KEY_4: return GLFW_KEY_4;
		case KeyboardKey::KEY_5: return GLFW_KEY_5;
		case KeyboardKey::KEY_6: return GLFW_KEY_6;
		case KeyboardKey::KEY_7: return GLFW_KEY_7;
		case KeyboardKey::KEY_8: return GLFW_KEY_8;
		case KeyboardKey::KEY_9: return GLFW_KEY_9;
		case KeyboardKey::KEY_NUMPAD_0: return GLFW_KEY_KP_0;
		case KeyboardKey::KEY_NUMPAD_1: return GLFW_KEY_KP_1;
		case KeyboardKey::KEY_NUMPAD_2: return GLFW_KEY_KP_2;
		case KeyboardKey::KEY_NUMPAD_3: return GLFW_KEY_KP_3;
		case KeyboardKey::KEY_NUMPAD_4: return GLFW_KEY_KP_4;
		case KeyboardKey::KEY_NUMPAD_5: return GLFW_KEY_KP_5;
		case KeyboardKey::KEY_NUMPAD_6: return GLFW_KEY_KP_6;
		case KeyboardKey::KEY_NUMPAD_7: return GLFW_KEY_KP_7;
		case KeyboardKey::KEY_NUMPAD_8: return GLFW_KEY_KP_8;
		case KeyboardKey::KEY_NUMPAD_9: return GLFW_KEY_KP_9;
		case KeyboardKey::KEY_SPACE: return GLFW_KEY_SPACE;
		case KeyboardKey::KEY_APOSTROPHE: return GLFW_KEY_APOSTROPHE;
		case KeyboardKey::KEY_COMMA: return GLFW_KEY_COMMA;
		case KeyboardKey::KEY_MINUS: return GLFW_KEY_MINUS;
		case KeyboardKey::KEY_PERIOD: return GLFW_KEY_PERIOD;
		case KeyboardKey::KEY_SLASH: return GLFW_KEY_SLASH;
		case KeyboardKey::KEY_SEMICOLON: return GLFW_KEY_SEMICOLON;
		case KeyboardKey::KEY_EQUAL: return GLFW_KEY_EQUAL;
		case KeyboardKey::KEY_LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;
		case KeyboardKey::KEY_RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;
		case KeyboardKey::KEY_BACKSLASH: return GLFW_KEY_BACKSLASH;
		case KeyboardKey::KEY_ESCAPE: return GLFW_KEY_ESCAPE;
		case KeyboardKey::KEY_ENTER: return GLFW_KEY_ENTER;
		case KeyboardKey::KEY_TAB: return GLFW_KEY_TAB;
		case KeyboardKey::KEY_BACKSPACE: return GLFW_KEY_BACKSPACE;
		case KeyboardKey::KEY_INSERT: return GLFW_KEY_INSERT;
		case KeyboardKey::KEY_DELETE: return GLFW_KEY_DELETE;
		case KeyboardKey::KEY_RIGHT: return GLFW_KEY_RIGHT;
		case KeyboardKey::KEY_LEFT: return GLFW_KEY_LEFT;
		case KeyboardKey::KEY_DOWN: return GLFW_KEY_DOWN;
		case KeyboardKey::KEY_UP: return GLFW_KEY_UP;
		case KeyboardKey::KEY_PAGE_UP: return GLFW_KEY_PAGE_UP;
		case KeyboardKey::KEY_PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
		case KeyboardKey::KEY_HOME: return GLFW_KEY_HOME;
		case KeyboardKey::KEY_END: return GLFW_KEY_END;
		case KeyboardKey::KEY_CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
		case KeyboardKey::KEY_SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
		case KeyboardKey::KEY_NUM_LOCK: return GLFW_KEY_NUM_LOCK;
		case KeyboardKey::KEY_PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
		case KeyboardKey::KEY_PAUSE: return GLFW_KEY_PAUSE;
		case KeyboardKey::KEY_LEFT_SHIFT: return GLFW_KEY_LEFT_SHIFT;
		case KeyboardKey::KEY_RIGHT_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
		case KeyboardKey::KEY_LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
		case KeyboardKey::KEY_RIGHT_CONTROL: return GLFW_KEY_RIGHT_CONTROL;
		case KeyboardKey::KEY_LEFT_ALT: return GLFW_KEY_LEFT_ALT;
		case KeyboardKey::KEY_RIGHT_ALT: return GLFW_KEY_RIGHT_ALT;
		case KeyboardKey::KEY_A: return GLFW_KEY_A;
		case KeyboardKey::KEY_B: return GLFW_KEY_B;
		case KeyboardKey::KEY_C: return GLFW_KEY_C;
		case KeyboardKey::KEY_D: return GLFW_KEY_D;
		case KeyboardKey::KEY_E: return GLFW_KEY_E;
		case KeyboardKey::KEY_F: return GLFW_KEY_F;
		case KeyboardKey::KEY_G: return GLFW_KEY_G;
		case KeyboardKey::KEY_H: return GLFW_KEY_H;
		case KeyboardKey::KEY_I: return GLFW_KEY_I;
		case KeyboardKey::KEY_J: return GLFW_KEY_J;
		case KeyboardKey::KEY_K: return GLFW_KEY_K;
		case KeyboardKey::KEY_L: return GLFW_KEY_L;
		case KeyboardKey::KEY_M: return GLFW_KEY_M;
		case KeyboardKey::KEY_N: return GLFW_KEY_N;
		case KeyboardKey::KEY_O: return GLFW_KEY_O;
		case KeyboardKey::KEY_P: return GLFW_KEY_P;
		case KeyboardKey::KEY_Q: return GLFW_KEY_Q;
		case KeyboardKey::KEY_R: return GLFW_KEY_R;
		case KeyboardKey::KEY_S: return GLFW_KEY_S;
		case KeyboardKey::KEY_T: return GLFW_KEY_T;
		case KeyboardKey::KEY_U: return GLFW_KEY_U;
		case KeyboardKey::KEY_V: return GLFW_KEY_V;
		case KeyboardKey::KEY_W: return GLFW_KEY_W;
		case KeyboardKey::KEY_X: return GLFW_KEY_X;
		case KeyboardKey::KEY_Y: return GLFW_KEY_Y;
		case KeyboardKey::KEY_Z: return GLFW_KEY_Z;
		case KeyboardKey::KEY_F1: return GLFW_KEY_F1;
		case KeyboardKey::KEY_F2: return GLFW_KEY_F2;
		case KeyboardKey::KEY_F3: return GLFW_KEY_F3;
		case KeyboardKey::KEY_F4: return GLFW_KEY_F4;
		case KeyboardKey::KEY_F5: return GLFW_KEY_F5;
		case KeyboardKey::KEY_F6: return GLFW_KEY_F6;
		case KeyboardKey::KEY_F7: return GLFW_KEY_F7;
		case KeyboardKey::KEY_F8: return GLFW_KEY_F8;
		case KeyboardKey::KEY_F9: return GLFW_KEY_F9;
		case KeyboardKey::KEY_F10: return GLFW_KEY_F10;
		case KeyboardKey::KEY_F11: return GLFW_KEY_F11;
		case KeyboardKey::KEY_F12: return GLFW_KEY_F12;
		default: return GLFW_KEY_UNKNOWN;
		}
		return GLFW_KEY_UNKNOWN;
	}

	std::int32_t WindowGLFW::ToGLFWMouseButton(const MouseButton & button) {
		switch (button) {
		case MouseButton::MOUSE_BUTTON_1: return GLFW_MOUSE_BUTTON_1;
		case MouseButton::MOUSE_BUTTON_2: return GLFW_MOUSE_BUTTON_2;
		case MouseButton::MOUSE_BUTTON_3: return GLFW_MOUSE_BUTTON_3;
		case MouseButton::MOUSE_BUTTON_4: return GLFW_MOUSE_BUTTON_4;
		case MouseButton::MOUSE_BUTTON_5: return GLFW_MOUSE_BUTTON_5;

		default: return -1;
		}
		return -1;
	}

	bool WindowGLFW::KeyPressed(const KeyboardKey & key) {
		std::int32_t glfwKey = ToGLFWKey(key);
		if(glfwKey == GLFW_KEY_UNKNOWN) return false;
		std::int32_t state = glfwGetKey(window, glfwKey);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}

	bool WindowGLFW::MouseButtonPressed(const MouseButton & button) {
		std::int32_t glfwButton = ToGLFWMouseButton(button);
		if (glfwButton == -1) return false;
		std::int32_t state = glfwGetMouseButton(window, glfwButton);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}

	void WindowGLFW::ProcessEvent(const Event & toProcess) {
		switch (toProcess.GetType()) {
		case EventType::EVENT_TYPE_SWAP:
			if (window) {

				glfwSwapBuffers(window);
				glfwPollEvents();
				const std::int32_t NUMBER_OF_KEYS = static_cast<std::int32_t>(KeyboardKey::NUMBER_OF_KEYS);
				InputInfo inputInfo;
				for (std::int32_t i = 0; i < NUMBER_OF_KEYS; i++) {
					inputInfo.keys[i] = KeyPressed((KeyboardKey)i);
				}
				
				const std::int32_t NUMBER_OF_BUTTONS = static_cast<std::int32_t>(MouseButton::NUMBER_OF_MOUSE_BUTTONS);
				
				for (std::int32_t i = 0; i < static_cast<std::int32_t>(NUMBER_OF_BUTTONS); i++) {
					inputInfo.buttons[i] = MouseButtonPressed((MouseButton)i);
				}
				double x = 0.0, y = 0.0;
				glfwGetCursorPos(window, &x, &y);
				inputInfo.x = (float)x;
				inputInfo.y = (float)y;
				if (glfwWindowShouldClose(window)) {
					MessagingSystem::GetInstance().SendEvent(Event(EventType::EVENT_TYPE_NULL, EventTarget::TARGET_SYSTEM_SHUTDOWN_OBSERVERS));
				}

				Event e = Event(EventType::EVENT_TYPE_UPDATE, EventTarget::TARGET_INPUT_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_INPUT_INFO, std::any(ObjectContainer<InputInfo>(inputInfo)));
				MessagingSystem::GetInstance().SendEvent(e);
			}
			else std::cout << "Window is NULL" << std::endl;
			break;
		case EventType::EVENT_TYPE_NULL:
			std::cout << "Null Event Type Sent To Window" << std::endl;
			
			break;
		default:
			std::cout << "The GLFW window couldn't process an event!" << std::endl;
			break;
		}
	}

}
