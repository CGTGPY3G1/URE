#ifndef UNNAMED_CULLING_SYSTEM_H
#define UNNAMED_CULLING_SYSTEM_H
#include <unordered_map>

#include "Emitter.h"
namespace B00289996 {
		enum BoundingType {
			BOUNDING_TYPE_SPHERE,
			BOUNDING_TYPE_AABB,
			BOUNDING_TYPE_OBB
		};
	class Event;
	class Camera;
	class Node;
	class MeshRenderer;
	struct CullingResult {
		std::shared_ptr<Node> camera;
		std::vector<std::shared_ptr<Node>> visibleNodes;
	};
	class CullingSystem : public ConnectionMananger {
		friend class Engine;
	public:
		CullingSystem();
		~CullingSystem();
		const std::vector<CullingResult> GetResults();
		const BoundingType GetBoundingType() const;
		void SetBoundingType(const BoundingType & type);
	protected:
		void ProcessEvent(const Event & toProcess);
		void ProcessDeregistrationEvent(const Event & toProcess);
		void ProcessRegistrationEvent(const Event & toProcess);
	private:
		bool performCulling;
		BoundingType collisionType;
		std::unordered_map<std::uint32_t, std::weak_ptr<Node>> trackedNodes;
		std::vector<std::weak_ptr<Camera>> cameras;
	};
}

#endif // !UNNAMED_CULLING_SYSTEM_H
