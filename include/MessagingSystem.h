#ifndef UNNAMED_MESSAGING_SYSTEM_H
#define UNNAMED_MESSAGING_SYSTEM_H
#include <queue>
#include "Emitter.h"
#include <any>
namespace B00289996 {
	enum EventDataType {
		EVENT_DATA_NULL,
		EVENT_DATA_NODE,
		EVENT_DATA_CAMERA,
		EVENT_DATA_TEXTURE,
		EVENT_DATA_MESH,
		EVENT_DATA_SHADER_PROGRAM,
		EVENT_DATA_VULKAN_INSTANCE,
		EVENT_DATA_INPUT_INFO,
		EVENT_DATA_UVEC2,
		EVENT_DATA_VEC2
	};

	enum EventTarget {
		TARGET_RENDERING_SYSTEM,
		TARGET_WINDOWING_SYSTEM,
		TARGET_CULLING_SYSTEM,
		TARGET_LIGHTING_SYSTEM,
		TARGET_INPUT_SYSTEM,
		TARGET_RENDERABLE_OBJECTS,
		TARGET_WINDOW_SIZE_OBSERVERS,
		TARGET_SYSTEM_SHUTDOWN_OBSERVERS,
		TARGET_ALL
	};

	static const std::string ToString(const EventTarget & target) {
		switch (target) {
		case EventTarget::TARGET_RENDERING_SYSTEM: return "TARGET_RENDERING_SYSTEM"; break;	
		case EventTarget::TARGET_WINDOWING_SYSTEM: return "TARGET_WINDOWING_SYSTEM"; break;
		case EventTarget::TARGET_CULLING_SYSTEM: return "TARGET_CULLING_SYSTEM"; break;
		case EventTarget::TARGET_LIGHTING_SYSTEM: return "TARGET_LIGHTING_SYSTEM"; break;
		case EventTarget::TARGET_INPUT_SYSTEM: return "TARGET_INPUT_SYSTEM"; break;
		case EventTarget::TARGET_RENDERABLE_OBJECTS: return "TARGET_RENDERABLE_OBJECTS"; break;
		case EventTarget::TARGET_WINDOW_SIZE_OBSERVERS: return "TARGET_WINDOW_SIZE_OBSERVERS"; break;
		case EventTarget::TARGET_SYSTEM_SHUTDOWN_OBSERVERS: return "TARGET_SYSTEM_SHUTDOWN_OBSERVERS"; break;
		case EventTarget::TARGET_ALL: return "TARGET_ALL"; break;
		default: return "UNKNOWN_SYSTEM"; break;
		}
	}

	static const char * ToChar(const EventTarget & target) {
		switch (target) {
		case EventTarget::TARGET_RENDERING_SYSTEM: return "TARGET_RENDERING_SYSTEM"; break;
		case EventTarget::TARGET_WINDOWING_SYSTEM: return "TARGET_WINDOWING_SYSTEM"; break;
		case EventTarget::TARGET_CULLING_SYSTEM: return "TARGET_CULLING_SYSTEM"; break;
		case EventTarget::TARGET_LIGHTING_SYSTEM: return "TARGET_LIGHTING_SYSTEM"; break;
		case EventTarget::TARGET_INPUT_SYSTEM: return "TARGET_INPUT_SYSTEM"; break;
		case EventTarget::TARGET_RENDERABLE_OBJECTS: return "TARGET_RENDERABLE_OBJECTS"; break;
		case EventTarget::TARGET_WINDOW_SIZE_OBSERVERS: return "TARGET_WINDOW_SIZE_OBSERVERS"; break;
		case EventTarget::TARGET_SYSTEM_SHUTDOWN_OBSERVERS: return "TARGET_SYSTEM_SHUTDOWN_OBSERVERS"; break;
		case EventTarget::TARGET_ALL: return "TARGET_ALL"; break;
		default: return "UNKNOWN_SYSTEM"; break;
		}
	}

	enum EventType {
		EVENT_TYPE_NULL,
		EVENT_TYPE_REGISTER,
		EVENT_TYPE_UNREGISTER,
		EVENT_TYPE_UPDATE,
		EVENT_TYPE_SWAP,
		EVENT_TYPE_TOGGLE
	};
	enum EventPriority {
		EVENT_SEND_IMMEDIATELY,
		EVENT_ADD_QUEUE
	};
	class Event {
	public:
		Event(const EventType & type = EventType::EVENT_TYPE_NULL, const EventTarget & target = EventTarget::TARGET_ALL, const EventPriority & priority = EventPriority::EVENT_SEND_IMMEDIATELY, const EventDataType & dataType = EventDataType::EVENT_DATA_NULL, std::any data = std::any())
			: mType(type), mTarget(target), mPriority(priority), mDataType(dataType), mData(data) {

		}
		~Event() {}
		const EventType GetType() const;
		const EventDataType GetDataType() const;
		const EventPriority GetPriority() const;
		const EventTarget GetTarget() const;
		const std::any GetData() const {
			return mData;
		}
	private:
		EventType mType;
		EventDataType mDataType;
		EventTarget mTarget;
		EventPriority mPriority;
		std::any mData;
	};

	class MessagingSystem {
	public:
		static MessagingSystem & GetInstance();
		/// <summary>Finalizes an instance of the <see cref="MessagingSystem"/> class.</summary>
		~MessagingSystem() {}
		void SendEvent(const Event & event);
		void ProcessQueue(const EventTarget & target = EventTarget::TARGET_ALL);
		std::shared_ptr<Connection> AttachObserver(const std::function<void(const Event)> & reqest, const EventTarget target);
	private:
		MessagingSystem();
		std::queue<Event> events;
		std::unordered_map<EventTarget, std::shared_ptr<Emitter<const Event>>> dispatchers;
	};
}

#endif // !UNNAMED_MESSAGING_SYSTEM_H