#include "Input.h"
#include "MessagingSystem.h"
#include "TypeInfo.h"
namespace B00289996 {
	Input::Input() : ConnectionMananger() {
		AddConnection(MessagingSystem::GetInstance().AttachObserver(std::bind(&Input::ProcessEvent, this, std::placeholders::_1), EventTarget::TARGET_INPUT_SYSTEM));
	}
	Input::~Input() {
	}
	bool Input::GetKeyDown(const KeyboardKey & key) {
		std::size_t index = static_cast<std::size_t>(key);
		if(index < static_cast<std::size_t>(KeyboardKey::NUMBER_OF_KEYS)) return pressed[index] && changed[index]; // true if key is pressed and was changed
		return false;
	}
	bool Input::GetKey(const KeyboardKey & key) {
		std::size_t index = static_cast<std::size_t>(key);
		if (index < static_cast<std::size_t>(KeyboardKey::NUMBER_OF_KEYS)) return pressed[index]; // true if key is pressed
		return false;
	}
	bool Input::GetKeyUp(const KeyboardKey & key) {
		std::size_t index = static_cast<std::size_t>(key);
		if (index < static_cast<std::size_t>(KeyboardKey::NUMBER_OF_KEYS)) return !pressed[index] && changed[index]; // true if key is not pressed and was changed
		return false;
	}
	bool Input::GetMouseButtonDown(const MouseButton & button) {
		std::size_t index = static_cast<std::size_t>(button);
		if (index < static_cast<std::size_t>(MouseButton::NUMBER_OF_MOUSE_BUTTONS)) return buttonPressed[index] && buttonChanged[index]; // true if button is pressed and was changed
		return false;
	}
	bool Input::GetMouseButton(const MouseButton & button) {
		std::size_t index = static_cast<std::size_t>(button);
		if (index < static_cast<std::size_t>(MouseButton::NUMBER_OF_MOUSE_BUTTONS)) return buttonPressed[index]; // true if key is pressed
		return false;
	}
	bool Input::GetMouseButtonUp(const MouseButton & button) {
		std::size_t index = static_cast<std::size_t>(button);
		if (index < static_cast<std::size_t>(MouseButton::NUMBER_OF_MOUSE_BUTTONS)) return !buttonPressed[index] && buttonChanged[index]; // true if key is not pressed and was changed
		return false;
	}
	glm::vec2 Input::GetMousePosition() {
		return mousePosition;
	}
	void Input::ProcessEvent(const Event & toProcess) {
		if (toProcess.GetType() == EventType::EVENT_TYPE_UPDATE) {
			if (toProcess.GetDataType() == EventDataType::EVENT_DATA_INPUT_INFO) {
				InputInfo inputInfo = std::any_cast<ObjectContainer<InputInfo>>(toProcess.GetData()).GetObject();
				for (size_t i = 0; i < (size_t)KeyboardKey::NUMBER_OF_KEYS; i++) {
					if (pressed[i] != inputInfo.keys[i]) {
						changed[i] = true;
						pressed[i] = inputInfo.keys[i];
					}
					else changed[i] = false;
				}
				for (size_t i = 0; i < (size_t)MouseButton::NUMBER_OF_MOUSE_BUTTONS; i++) {
					if (buttonPressed[i] != inputInfo.buttons[i]) {
						buttonChanged[i] = true;
						buttonPressed[i] = inputInfo.buttons[i];
					}
					else buttonChanged[i] = false;
				}
				mousePosition = glm::vec2(inputInfo.x, inputInfo.y);
			}
		}
	}
}
