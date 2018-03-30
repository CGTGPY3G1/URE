#include "MessagingSystem.h"
#include <iostream>
namespace B00289996 {
	MessagingSystem & MessagingSystem::GetInstance() {
		static MessagingSystem instance;
		return instance;
	}

	void MessagingSystem::SendEvent(const Event & toSend) {
		switch (toSend.GetPriority()) {
		case EventPriority::EVENT_SEND_IMMEDIATELY:
			if (dispatchers[toSend.GetTarget()]) dispatchers[toSend.GetTarget()]->Emit(std::forward<const Event>(toSend));
			else std::cout << "Attempted to send message to null " << ToChar(toSend.GetTarget()) << "!" << std::endl;
			break;
		case EventPriority::EVENT_ADD_QUEUE:
			events.push(toSend);
			break;
		}
	}

	void MessagingSystem::ProcessQueue(const EventTarget & target) {
		while (events.size() > 0) {
			Event toSend = events.front();

			if (target == EventTarget::TARGET_ALL || target == toSend.GetTarget()) {
				if (dispatchers.count(toSend.GetTarget()) > 0) dispatchers[toSend.GetTarget()]->Emit(std::forward<const Event>(toSend));
				else std::cout << "Attempted to send message to null target!" << std::endl;
				events.pop();
			}
		}
	}

	std::shared_ptr<Connection> MessagingSystem::AttachObserver(const std::function<void(const Event)> & reqest, const EventTarget target) {
		std::cout << "attached observer to " << ToChar(target) << std::endl;
		if (!dispatchers[target]) dispatchers[target] = std::make_shared<Emitter<const Event>>(ToString(target));
		return dispatchers[target]->Connect(reqest);
	}

	MessagingSystem::MessagingSystem() : dispatchers(std::unordered_map<EventTarget, std::shared_ptr<Emitter<const Event>>>()) {}

	const EventType Event::GetType() const {
		return mType;
	}
	const EventDataType Event::GetDataType() const {
		return mDataType;
	}
	const EventPriority Event::GetPriority() const {
		return mPriority;
	}
	const EventTarget Event::GetTarget() const {
		return mTarget;
	}

}
