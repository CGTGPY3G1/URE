#ifndef UNNAMED_INPUT_H
#define UNNAMED_INPUT_H
#include <glm/vec2.hpp>
#include <memory>
#include "Enums.h"
#include "Emitter.h"
namespace B00289996 {
	class Event;
	class Input : public ConnectionMananger {
		friend class Engine;
	public:
		Input();
		~Input();
		/// <summary>Returns true if the key was pressed during the last frame.</summary>
		/// <param name="scanCode">The scan code.</param>
		/// <returns>true if the key was pressed during the last frame, else false</returns>
		bool GetKeyDown(const KeyboardKey & key);
		/// <summary>Returns true if the key was held down during the last frame.</summary>
		/// <param name="scanCode">The scan code.</param>
		/// <returns>true if the key was held down during the last frame, else false</returns>
		bool GetKey(const KeyboardKey & key);
		/// <summary> Returns true if the key was released during the last frame. </summary>
		/// <param name="scanCode">The scan code.</param>
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
		/*/// <summary> Gets the mouse movement.</summary>
		/// <returns>the mouse movement delta</returns>
		glm::vec2 GetMouseMovement();*/
	private:
		void ProcessEvent(const Event & toProcess);
		bool pressed[(std::size_t)KeyboardKey::NUMBER_OF_KEYS]; // array of flags for key presses
		bool changed[(std::size_t)KeyboardKey::NUMBER_OF_KEYS]; // array of flags for key press changes
		bool buttonPressed[(std::size_t)MouseButton::NUMBER_OF_MOUSE_BUTTONS]; // array of flags for key presses
		bool buttonChanged[(std::size_t)MouseButton::NUMBER_OF_MOUSE_BUTTONS]; // array of flags for key press changes
		glm::vec2 mousePosition;
		//bool mousePressed[5]; // array of flags for mouse presses
		//bool mouseChanged[5]; // array of flags for mouse press changes
		//glm::vec2 mousePosition, oldMousePosition; // used to track mouse position and motion
	};
}

#endif // !UNNAMED_INPUT_H
