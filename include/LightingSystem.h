#ifndef UNNAMED_LIGHTING_SYSTEM_H
#define UNNAMED_LIGHTING_SYSTEM_H
#include <unordered_map>

#include "Emitter.h"
namespace B00289996 {

	class Event;
	class Node;
	class LightingSystem : public ConnectionMananger {
	public:
		LightingSystem();
		~LightingSystem();
		std::vector<std::shared_ptr<Node>> GetAllLights();
	protected:
		void ProcessEvent(const Event & toProcess);
		void ProcessDeregistrationEvent(const Event & toProcess);
		void ProcessRegistrationEvent(const Event & toProcess);

	private:
		std::vector<std::weak_ptr<Node>> lights;
	};
}

#endif // !UNNAMED_LIGHTING_SYSTEM_H
