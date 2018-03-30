#ifndef UNNAMED_RENDERABLE_RESOURCE_H
#define UNNAMED_RENDERABLE_RESOURCE_H
#include "Object.h"
#include "MessagingSystem.h"
namespace B00289996 {
	class RenderableResource : public SGObject {
		friend class FileInput;
	public:
		RenderableResource();
		~RenderableResource();
		
	protected:
		void ProcessEvent(const Event & toProcess) override;
		void PushToRenderer() {
			MessagingSystem::GetInstance().SendEvent(eventData);
		}
		void ObserveRenderSwitch() {
			AddConnection(MessagingSystem::GetInstance().AttachObserver(std::bind(&RenderableResource::ProcessEvent, this, std::placeholders::_1), EventTarget::TARGET_RENDERABLE_OBJECTS));
		}
		Event eventData;
	};

}
#endif // !UNNAMED_RENDERABLE_RESOURCE_H

