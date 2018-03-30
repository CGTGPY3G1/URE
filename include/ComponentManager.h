#ifndef UNNAMED_COMPONENT_MANAGER_H
#define UNNAMED_COMPONENT_MANAGER_H
#include "Component.h"
#include <utility> 
#include <memory>
#include <vector>
#include <map>
namespace B00289996 {

	class ComponentManager {
	public:
		ComponentManager();
		~ComponentManager();

	private:
		std::map<std::size_t, std::vector<Component>> components;
	};
	
}

#endif // !UNNAMED_COMPONENT_MANAGER_H
