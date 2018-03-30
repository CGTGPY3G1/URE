#ifndef UNNAMED_COMPONENT_H
#define UNNAMED_COMPONENT_H
#include "Object.h"
#include "TypeInfo.h"
namespace B00289996 {
	class Node;
	class Component : public SGObject {
		friend class Node;
		friend struct std::hash<Component>;
		friend struct std::hash<std::shared_ptr<Component>>;
	public:
		Component();
		~Component();
		template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type> std::weak_ptr<T> GetComponent();
		template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type> std::vector<std::weak_ptr<T>> GetComponents();
		virtual const BitMask GetComponentID() const {
			return TypeInfo::GetComponentID<Component>();
		}

		virtual const std::shared_ptr<Component> Clone() const = 0;
		const std::weak_ptr<Node> GetOwner();
		virtual void SetOwner(const std::weak_ptr<Node> & owner);
	protected:
		std::weak_ptr<Node> owner;
	private:
		std::size_t id;
	};

	template <typename ComponentType = std::enable_if<std::is_base_of<Component, ComponentType>::value>::type>
	static const std::shared_ptr<ComponentType> Clone(const std::shared_ptr<ComponentType> & c) {
		std::shared_ptr<ComponentType> toReturn = std::make_shared<ComponentType>(c);
		toReturn->SetOwner(std::shared_ptr<Node>());
		return std::static_pointer_cast<Component>(toReturn);
	}

	template<typename T> std::weak_ptr<T> Component::GetComponent() {
		std::shared_ptr<Node> n = owner.lock();
		if (n) return n->GetComponent<T>();
		return std::weak_ptr<T>();
	}

	template<typename T> std::vector<std::weak_ptr<T>> Component::GetComponents() {
		std::shared_ptr<Node> n = owner.lock();
		if (n) return n->GetComponents<T>();
		return std::vector<std::weak_ptr<T>>();
	}

}
namespace std {
	template <>
	struct hash<B00289996::Component> {
		std::size_t operator()(const B00289996::Component& k) const {
			return k.id;
		}
	};
	
	template <>
	struct hash<std::shared_ptr<B00289996::Component>> {
		std::size_t operator()(const std::shared_ptr<B00289996::Component>& k) const {
			return k->id;
		}
	};
}
#endif // !UNNAMED_COMPONENT_H
