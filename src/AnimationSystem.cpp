#include "AnimationSystem.h"
#include "Animator.h"
namespace B00289996 {
	B00289996::AnimationSystem::AnimationSystem() {
	}

	B00289996::AnimationSystem::~AnimationSystem() {
	}

	std::vector<std::shared_ptr<Animator>> AnimationSystem::GetAllAnimators() {
		if (!animators.empty()) {
			std::vector<std::shared_ptr<Animator>> toReturn;
			for (std::vector<std::weak_ptr<Animator>>::iterator i = animators.begin(); i != animators.end(); i++) {
				if (!(*i).expired()) {
					animators.push_back((*i).lock());
				}
			}
			return toReturn;
		}
		return std::vector<std::shared_ptr<Animator>>();
	}

	void B00289996::AnimationSystem::ProcessEvent(const Event & toProcess) {
	}

	void B00289996::AnimationSystem::ProcessDeregistrationEvent(const Event & toProcess) {
	}

	void B00289996::AnimationSystem::ProcessRegistrationEvent(const Event & toProcess) {
	}

}