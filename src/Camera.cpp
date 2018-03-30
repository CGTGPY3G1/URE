#include "Camera.h"
#include "Node.h"
#include "Engine.h"
#include "IDGenerator.h"
#include "MessagingSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
namespace B00289996 {
	
	/// <summary>
	/// The main camera
	/// </summary>
	std::shared_ptr<Camera> Camera::mainCamera;

	Camera::Camera(const float & width, const float & height, const float & near, const float & far, const float & fov) : Component(), dirty(true),
		registeredWithCulling(false), position(glm::vec3()), forward(glm::vec3()), right(glm::vec3()), up(glm::vec3()) {
		rendererType = Engine::selectedRenderer;
		SetProjection(width, height, near, far, fov);
		id = IDGenerator<Camera, std::uint16_t>::GetInstance().GetNewID();
	}

	Camera::~Camera() {
		//RegisterWithCullingSystem(false);
		IDGenerator<Camera, std::uint16_t>::GetInstance().RetireID(id);
	}

	void Camera::SetProjection(const float & width, const float & height, const float & near, const float & far, const float & fov) {
		this->width = width; this->height = height; this->near = near; this->far = far; this->fov = fov;
		const float aspect = width / height;
		const float rFOV = glm::radians(fov);
		rendererType = Engine::selectedRenderer;
		projection = glm::perspective(rFOV, aspect, near, far) ;
		frustum.DefinePerspective(rFOV, aspect, near, far);
	}

	void Camera::SetAsMain() {
		 Camera::mainCamera = std::static_pointer_cast<Camera>(shared_from_this());
	}

	bool Camera::IsMainCamera() {
		return (mainCamera && std::static_pointer_cast<Camera>(shared_from_this()) == mainCamera);
	}

	std::weak_ptr<Camera> Camera::GetMainCamera() {
		return mainCamera;
	}

	bool Camera::operator==(const Camera & other) {
		return objectID == other.objectID;
	}

	bool Camera::operator==(const std::shared_ptr<Camera>& other) {
		return objectID == other->objectID;
	}

	const float Camera::GetWidth() const {
		return width;
	}

	const float Camera::GetHeight() const {
		return height;
	}

	const glm::mat4 Camera::GetView() {
		UpdateView();
		return view;
	}

	const glm::mat4 Camera::GetProjection() {
		if (rendererType != Engine::selectedRenderer) SetProjection(width, height, near, far, fov);
		return projection;
	}

	const glm::mat4 Camera::GetViewProjection() {
		return GetProjection() * GetView();
	}

	const Frustum Camera::GetFrustum() {
		UpdateView();
		return frustum;
	}

	const glm::vec3 Camera::GetPosition() {
		UpdateView();
		return position;
	}

	const glm::vec3 Camera::GetForward() {
		UpdateView();
		return forward;
	}

	const glm::vec3 Camera::GetRight() {
		UpdateView();
		return right;
	}

	const glm::vec3 Camera::GetUp() {
		UpdateView();
		return up;
	}

	void Camera::SetOwner(const std::weak_ptr<Node>& owner) {
		if (owner.use_count() > 0) {
			RegisterWithCullingSystem(false);
		}
		Component::SetOwner(owner);
		if (owner.use_count() > 0) {
			RegisterWithCullingSystem(true);
			AddConnection(owner.lock()->AttachTransformUpdateObserver(std::bind(&Camera::SetDirty, this, std::placeholders::_1)));
			SetDirty(true);
		}
	}

	const std::uint16_t Camera::GetID() const {
		return id;
	}

	const Viewport Camera::GetViewport() const {
		return viewport;
	}

	void Camera::SetViewPort(const Viewport & newViewport) {
		viewport = newViewport;
	}

	void Camera::SetViewPort(const glm::vec2 & min, const glm::vec2 & max) {
		viewport.min = min; viewport.max = max;
	}

	void Camera::SetViewPort(const float & left, const float & bottom, const float & right, const float & top) {
		viewport.min = glm::vec2(left, bottom); viewport.max = glm::vec2(right, top);
	}

	void Camera::RegisterWithCullingSystem(const bool & connect) {
		if (!registeredWithCulling && connect) {
			std::any data;
			data.emplace<ObjectContainer<std::shared_ptr<Camera>>>(ObjectContainer<std::shared_ptr<Camera>>(std::static_pointer_cast<Camera>(shared_from_this())));
			Event e = Event(EventType::EVENT_TYPE_REGISTER, EventTarget::TARGET_CULLING_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_CAMERA, data);
			MessagingSystem::GetInstance().SendEvent(e);
			registeredWithCulling = true;
		}
		else if(registeredWithCulling && !connect) {
			std::any data;
			data.emplace<ObjectContainer<std::shared_ptr<Camera>>>(ObjectContainer<std::shared_ptr<Camera>>(std::static_pointer_cast<Camera>(shared_from_this())));
			Event e = Event(EventType::EVENT_TYPE_UNREGISTER, EventTarget::TARGET_CULLING_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_CAMERA, data);
			MessagingSystem::GetInstance().SendEvent(e);
			registeredWithCulling = false;
		}
	}

	void Camera::UpdateView() {
		if (this->dirty) {
			std::shared_ptr<Node> transform = GetOwner().lock();
			if (transform) {
				position = transform->GetPosition();
				forward = transform->GetForward(); right = transform->GetRight(); up = transform->GetUp();
				view = glm::lookAt(position, position + forward, up);
				frustum.DefineView(position, forward, up);
			}
			else {
				position = glm::vec3(0.0f);
				forward = glm::vec3(0.0f, 0.0f, -1.0f); right = glm::vec3(1.0f, 0.0f, 0.0f); up = glm::vec3(0.0f, 1.0f, 0.0f);
				view = glm::mat4(1.0f);
				frustum.DefineView(position, forward, up);
			}
			SetDirty(false);
		}
	}

	void Camera::SetDirty(const bool & newValue) {
		 dirty = newValue;
	}

}
