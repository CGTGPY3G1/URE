#include "Node.h"
#include <algorithm>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Bounds.h"
#include "MeshRenderer.h"
#include "ShaderProgram.h"
#include "IDGenerator.h"
namespace B00289996 {
	Node::Node(const std::string & nodeName) : Transformable(nodeName + "Transform"), name(nodeName), enabled(true), componentsDirty(true), components(std::vector<std::vector<std::shared_ptr<Component>>>()) {
		id = IDGenerator<Node, std::uint32_t>::GetInstance().GetNewID();
	}

	Node::~Node() {
		IDGenerator<Node, std::uint32_t>::GetInstance().RetireID(id);
	}
	const bool Node::HasAnyOfComponents(const BitMask & componentMask) const {
		return (bitmask & componentMask) != 0;
	}
	const bool Node::HasAllOfComponents(const BitMask & componentMask) const {
		return (bitmask & componentMask) == componentMask;
	}
	const std::weak_ptr<Node> Node::GetChild(const std::size_t index) const {
		return  std::static_pointer_cast<Node>(Transformable::GetChild(0).lock());
	}
	const std::string Node::GetName() const {
		return name;
	}

	void Node::SetName(const std::string & newName) {
		name = newName;
	}

	const std::uint32_t Node::GetID() const {
		return id;
	}


	void Node::SetEnabled(const bool & enabled) {
		if (this->enabled != enabled) this->enabled = enabled;
		for (size_t i = 0; i < children.size(); i++) {
			std::shared_ptr<Node> c = std::static_pointer_cast<Node>(children[i].lock());
			if (c) c->SetEnabled(enabled);
		}
	}

	void Node::ToggleEnabled() {
		SetEnabled(!enabled);
	}

	const bool Node::GetEnabled() const {
		return enabled;
	}
}
