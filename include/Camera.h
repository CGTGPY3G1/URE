#ifndef B00289996B00227422_CAMERA_H
#define B00289996B00227422_CAMERA_H
#include "Component.h"
#include "Frustum.h"
#include "Enums.h"
#include <glm/mat4x4.hpp>
#include <memory>

namespace B00289996 {

	struct Viewport {
		glm::vec2 min = glm::vec2(0.0f), max = glm::vec2(1.0f);
	};
	class Camera : public Component {
		friend class Node;
	public:
		/// <summary>Initializes a new instance of the <see cref="Camera"/> class.</summary>
		Camera(const float & width = 1280, const float & height = 720, const float & near = 0.1f, const float & far = 200.0f, const float & fov = 60.0f);
		/// <summary>Finalizes an instance of the <see cref="Camera"/> class.</summary>
		~Camera();
		/// <summary>Sets the cameras viewprojection values.</summary>
		/// <param name="width">The width.</param>
		/// <param name="height">The height.</param>
		/// <param name="near">The near plane.</param>
		/// <param name="">The far plane.</param>
		/// <param name="fov">The field of view.</param>
		void SetProjection(const float & width = 1280.0f, const float & height = 720.0f, const float & near = 0.1f, const float & far = 200.0f, const float & fov = 60.0f);
		/// <summary>Sets this camera as the main camera. allowing it to be accessed useing Camera::mainCamera or Camera::GetMainCamera()</summary>
		void SetAsMain();
		/// <summary>Determines whether this is the main camera or not</summary>
		/// <returns>true is this is the main camera, else false</returns>
		bool IsMainCamera();
		/// <summary>Gets the main camera.</summary>
		/// <returns>the main camera if there is one, else an empty weak pointer</returns>
		static std::weak_ptr<Camera> GetMainCamera();
		bool operator == (const Camera & other);
		bool operator == (const std::shared_ptr<Camera> & other);
		/// <summary>Gets the cameras width.</summary>
		/// <returns>the width</returns>
		const float GetWidth() const;
		/// <summary>Gets the cameras height.</summary>
		/// <returns>the cameras height</returns>
		const float GetHeight() const;
		/// <summary>Gets the view matrix of this camera.</summary>
		/// <returns>the view matrix</returns>
		const glm::mat4 GetView();
		/// <summary>Gets the viewprojection matrix of this camera.</summary>
		/// <returns>the viewprojection matrix</returns>
		const glm::mat4 GetProjection();
		/// <summary>Gets the combined view viewprojection matrix of this camera.</summary>
		/// <returns>the combined view & viewprojection matrix</returns>
		const glm::mat4 GetViewProjection();
		/// <summary>Gets the cameras frustum.</summary>
		/// <returns>the frustum</returns>
		const Frustum GetFrustum();
		const glm::vec3 GetPosition();
		const glm::vec3 GetForward();
		const glm::vec3 GetRight();
		const glm::vec3 GetUp();
		virtual const BitMask GetComponentID() const override {
			return TypeInfo::GetComponentID<Camera>();
		}
		virtual const std::shared_ptr<Component> Clone() const override {
			std::shared_ptr<Camera> toReturn = std::make_shared<Camera>(*this);
			toReturn->SetOwner(std::shared_ptr<Node>());
			return std::static_pointer_cast<Component>(toReturn);
		}
		void SetOwner(const std::weak_ptr<Node> & owner) override;
		const std::uint16_t GetID() const;
		const Viewport GetViewport() const;
		void SetViewPort(const Viewport & newViewport);
		void SetViewPort(const glm::vec2 & start, const glm::vec2 & extents);
		void SetViewPort(const float & x, const float & y, const float & width, const float & height);
	private:
		Viewport viewport;
		RendererType rendererType;
		void RegisterWithCullingSystem(const bool & connect);
		void UpdateView();
		void SetDirty(const bool & newValue);
		static std::shared_ptr<Camera> mainCamera;
		glm::mat4 view, projection, viewProjection; //  the cameras viewprojection matrix
		Frustum frustum;
		glm::vec3 position, forward, right, up;
		bool dirty, registeredWithCulling;
		// viewprojection values
		float width = 1280, height = 720, near = 0.1f, far = 50.0f, fov = 60.0f;
		std::uint16_t id;
	};
}

#endif // !B00289996B00227422_CAMERA_H