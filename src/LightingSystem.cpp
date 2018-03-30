#include "LightingSystem.h"
#include "MessagingSystem.h"
#include "Node.h"
#include <iostream>
namespace B00289996 {
	LightingSystem::LightingSystem() : ConnectionMananger(), lights(std::vector<std::weak_ptr<Node>>()){
		AddConnection(MessagingSystem::GetInstance().AttachObserver(std::bind(&LightingSystem::ProcessEvent, this, std::placeholders::_1), EventTarget::TARGET_LIGHTING_SYSTEM));
	}
	LightingSystem::~LightingSystem() {
	}
	std::vector<std::shared_ptr<Node>> LightingSystem::GetAllLights() {
		std::vector<std::shared_ptr<Node>> toReturn = std::vector<std::shared_ptr<Node>>();
		for (std::vector<std::weak_ptr<Node>>::iterator i = lights.begin(); i != lights.end(); ++i) {
			std::shared_ptr<Node> n = (*i).lock();
			if(n && n->GetEnabled()) toReturn.push_back(n);
		}
		return toReturn;
	}
	void LightingSystem::ProcessEvent(const Event & toProcess) {
		switch (toProcess.GetType()) {
		case EventType::EVENT_TYPE_REGISTER:
			ProcessRegistrationEvent(toProcess);
			break;
		case EventType::EVENT_TYPE_UNREGISTER:
			ProcessDeregistrationEvent(toProcess);
			break;

		case EventType::EVENT_TYPE_NULL:
			std::cout << "Null Event Type Sent To Culling System" << std::endl;
			break;
		default:
			break;
		}
	}
	void LightingSystem::ProcessDeregistrationEvent(const Event & toProcess) {
		if (toProcess.GetDataType() == EventDataType::EVENT_DATA_NODE) {
			std::shared_ptr<Node> node = std::any_cast<ObjectContainer<std::shared_ptr<Node>>>(toProcess.GetData()).GetObject();
			if (node) {
				for (std::vector<std::weak_ptr<Node>>::iterator i = lights.begin(); i != lights.end(); ++i) {
					std::shared_ptr<Node> storedNode = (*i).lock();
					if (storedNode) {
						if (storedNode->GetID() == node->GetID()) {
							lights.erase(i);
							break;
						}
					}
				}
			}
		}
	}
	void LightingSystem::ProcessRegistrationEvent(const Event & toProcess) {
		if (toProcess.GetDataType() == EventDataType::EVENT_DATA_NODE) {
			std::shared_ptr<Node> node = std::any_cast<ObjectContainer<std::shared_ptr<Node>>>(toProcess.GetData()).GetObject();
			bool alreadyAdded = false;
			for (std::vector<std::weak_ptr<Node>>::iterator i = lights.begin(); i != lights.end() && !alreadyAdded; ++i) {
				std::shared_ptr<Node> toCheck = (*i).lock();
				if (toCheck->GetID() == node->GetID()) {
					alreadyAdded = true;
				}
			}
			if (!alreadyAdded) lights.push_back(node);
		}
	}
}
