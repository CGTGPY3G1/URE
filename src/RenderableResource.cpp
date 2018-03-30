#include "RenderableResource.h"
namespace B00289996 {
	RenderableResource::RenderableResource() {
	}
	RenderableResource::~RenderableResource() {
	}
	void RenderableResource::ProcessEvent(const Event & toProcess) {
		switch (toProcess.GetType()) {
		case EventType::EVENT_TYPE_UPDATE:
			PushToRenderer();
			break;
		default:
			break;
		}
	}
}
