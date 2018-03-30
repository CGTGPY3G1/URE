#ifndef UNNAMED_LIGHTING_SYSTEM_H
#define UNNAMED_LIGHTING_SYSTEM_H
#include <unordered_map>

#include "Emitter.h"
namespace B00289996 {

	class Event;
	class Animator;
	class AnimationSystem : public ConnectionMananger {
	public:
		AnimationSystem();
		~AnimationSystem();
		std::vector<std::shared_ptr<Animator>> GetAllAnimators();
	protected:
		void ProcessEvent(const Event & toProcess);
		void ProcessDeregistrationEvent(const Event & toProcess);
		void ProcessRegistrationEvent(const Event & toProcess);
	private:
		std::vector<std::weak_ptr<Animator>> animators;
	};
}

#endif // !UNNAMED_LIGHTING_SYSTEM_H