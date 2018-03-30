#include "CullingSystem.h"
#include <iostream>
#include "MessagingSystem.h"
#include "MeshRenderer.h"
#include "Node.h"
#include "Camera.h"
#include "Frustum.h"
#include <chrono>

namespace B00289996 {
	CullingSystem::CullingSystem() : ConnectionMananger(), trackedNodes(std::unordered_map<std::uint32_t, std::weak_ptr<Node>>()), performCulling(true) {
		AddConnection(MessagingSystem::GetInstance().AttachObserver(std::bind(&CullingSystem::ProcessEvent, this, std::placeholders::_1), EventTarget::TARGET_CULLING_SYSTEM));
	}
	CullingSystem::~CullingSystem() {
	}
	const std::vector<CullingResult> CullingSystem::GetResults() {
		std::vector<CullingResult> toReturn = std::vector<CullingResult>();
		//std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		for (std::vector<std::weak_ptr<Camera>>::iterator i = cameras.begin(); i != cameras.end(); ++i) {
			std::shared_ptr<Camera> cam = (*i).lock();
			if (cam) {
				CullingResult result;
				result.camera = cam->GetOwner().lock();
				Frustum frustum = cam->GetFrustum();
				for (std::unordered_map<std::uint32_t, std::weak_ptr<Node>>::iterator j = trackedNodes.begin(); j != trackedNodes.end(); ++j) {
					std::shared_ptr<Node> node = (*j).second.lock();
					if (node && node->GetEnabled()) {
						if (performCulling) {
							std::shared_ptr<MeshRenderer> meshRenderer = node->GetComponent<MeshRenderer>();
							if (meshRenderer) {
								Sphere sphere = meshRenderer->GetSphere();
								if (frustum.CheckSphere(sphere)) {
									result.visibleNodes.push_back(node);
								}
							}
						}
						else {
							result.visibleNodes.push_back(node);
						}
					}
				}
				toReturn.push_back(result);
			}
		}
		/*std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration step = (endTime - currentTime);
		float stepAsFloat = float(step.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;
		std::cout << "Culling took " << std::to_string(stepAsFloat) << std::endl;*/
		return toReturn;
	}
	const BoundingType CullingSystem::GetBoundingType() const {
		return collisionType;
	}
	void CullingSystem::SetBoundingType(const BoundingType & type) {
		collisionType = type;
	}
	void CullingSystem::ProcessEvent(const Event & toProcess) {
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
	void CullingSystem::ProcessDeregistrationEvent(const Event & toProcess) {
		if (toProcess.GetDataType() == EventDataType::EVENT_DATA_NODE) {
			std::shared_ptr<Node> node = std::any_cast<ObjectContainer<std::shared_ptr<Node>>>(toProcess.GetData()).GetObject();
			if(node) trackedNodes.erase(node->GetID());
		}
		else if (toProcess.GetDataType() == EventDataType::EVENT_DATA_CAMERA) {
			std::shared_ptr<Camera> cam = std::any_cast<ObjectContainer<std::shared_ptr<Camera>>>(toProcess.GetData()).GetObject();
			for (std::vector<std::weak_ptr<Camera>>::iterator i = cameras.begin(); i != cameras.end(); ++i) {
				std::shared_ptr<Camera> toCheck = (*i).lock();
				if (toCheck->GetID() == cam->GetID()) {
					cameras.erase(i);
					break;
				}
			}
		}
	}
	void CullingSystem::ProcessRegistrationEvent(const Event & toProcess) {
		if (toProcess.GetDataType() == EventDataType::EVENT_DATA_NODE) {
			std::shared_ptr<Node> node = std::any_cast<ObjectContainer<std::shared_ptr<Node>>>(toProcess.GetData()).GetObject();
			if (node) trackedNodes[node->GetID()] = node;
		}
		else if (toProcess.GetDataType() == EventDataType::EVENT_DATA_CAMERA) {
			std::shared_ptr<Camera> cam = std::any_cast<ObjectContainer<std::shared_ptr<Camera>>>(toProcess.GetData()).GetObject();
			bool alreadyAdded = false;
			for (std::vector<std::weak_ptr<Camera>>::iterator i = cameras.begin(); i != cameras.end() && !alreadyAdded; ++i) {
				std::shared_ptr<Camera> toCheck = (*i).lock();
				if (toCheck->GetID() == cam->GetID()) {
					alreadyAdded = true;
				}
			}
			if (!alreadyAdded) cameras.push_back(cam);
		}
	}
}
