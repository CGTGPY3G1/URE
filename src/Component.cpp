#include "Component.h"
#include "IDGenerator.h"
namespace B00289996 {
	Component::Component() : SGObject(), owner(std::weak_ptr<Node>()) {
		id = IDGenerator<Component>::GetInstance().GetNewID();
	}
	Component::~Component() {
		IDGenerator<Component>::GetInstance().RetireID(id);
	}
	const std::weak_ptr<Node> Component::GetOwner() { return owner; }
	void Component::SetOwner(const std::weak_ptr<Node>& owner) { this->owner = owner; }
}
