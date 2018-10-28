#include <vector>
#include <memory>
namespace B00289996 {
	class Node;
	struct CullingResult {
		std::shared_ptr<Node> camera;
		std::vector<std::shared_ptr<Node>> visibleNodes;
		std::vector<std::shared_ptr<Node>> lights;
	};
}
