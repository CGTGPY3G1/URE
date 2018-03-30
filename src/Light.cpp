#include "Light.h"
#include "MessagingSystem.h"
namespace B00289996 {
	Light::Light() : Component(), registeredWithLightingSystem(false), lightType(POINT_LIGHT){
		light = std::make_shared<LightStruct>();
	}

	Light::~Light() {
		//RegisterWithLightingSystem(false);
	}

	const LightType Light::GetLightType() const {
		return lightType;
	}

	void Light::SetLightType(const LightType & newType) {
		lightType = newType;
	}

	void Light::SetOwner(const std::weak_ptr<Node>& owner) {
		if (owner.use_count() > 0) RegisterWithLightingSystem(false);
		Component::SetOwner(owner);
		if (owner.use_count() > 0) RegisterWithLightingSystem(true);
	}

	void Light::RegisterWithLightingSystem(const bool & connect) {
		if (!registeredWithLightingSystem && connect) {
			std::shared_ptr<Node> node = GetOwner().lock();
			if (node) {
				std::any data;
				data.emplace<ObjectContainer<std::shared_ptr<Node>>>(ObjectContainer<std::shared_ptr<Node>>(node));
				Event e = Event(EventType::EVENT_TYPE_REGISTER, EventTarget::TARGET_LIGHTING_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_NODE, data);
				MessagingSystem::GetInstance().SendEvent(e);
				registeredWithLightingSystem = true;
			}
		}
		else if (registeredWithLightingSystem && !connect) {
			std::shared_ptr<Node> node = GetOwner().lock();
			if (node) {
				std::any data;
				data.emplace<ObjectContainer<std::shared_ptr<Node>>>(ObjectContainer<std::shared_ptr<Node>>(node));
				Event e = Event(EventType::EVENT_TYPE_UNREGISTER, EventTarget::TARGET_LIGHTING_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_NODE, data);
				MessagingSystem::GetInstance().SendEvent(e);
				registeredWithLightingSystem = false;
			}
		}
	}

	
}
