#ifndef UNNAMED_NODE_H
#define UNNAMED_NODE_H
#include <string>
#include <memory>
#include <vector>
#include <map>
#include "Transformable.h"
#include "TypeInfo.h"
#include "BitMask.h"
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
namespace B00289996 {
	class Component;
	struct Sphere;
	struct AABB;
	struct OBB;
	class Node : public Transformable {
	public:

		Node(const std::string & nodeName = "UnnamedNode");
		~Node();
		template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type> std::shared_ptr<T> GetComponent();
		template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type> std::vector<std::shared_ptr<T>> GetComponents();
		template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type> std::shared_ptr<T> AddComponent(const std::shared_ptr<T> component);
		template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type> std::shared_ptr<T> AddComponent();
		template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type> const bool HasComponent() const;
		const bool HasAnyOfComponents(const BitMask & componentMask) const;
		const bool HasAllOfComponents(const BitMask & componentMask) const;
		//template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type> void RemoveComponent(const std::size_t & id);
		const std::weak_ptr<Node> GetChild(const std::size_t index) const;
		const std::string GetName() const;
		void SetName(const std::string & newName);
		const std::uint32_t GetID() const;
		void SetEnabled(const bool & enabled);
		void ToggleEnabled();
		const bool GetEnabled() const;
	protected:
		bool componentsDirty, enabled;
		std::uint32_t id;	
	private:
		std::string name;
		BitMask bitmask;
		std::map<std::size_t, std::vector<std::shared_ptr<Component>>> components;
	};

	template<typename T>
	inline std::shared_ptr<T> Node::GetComponent() {
		static BitMask id = TypeInfo::GetComponentID<T>();
		if (components.count(id.AsNumber()) > 0) {
			std::shared_ptr<Component> c = components[id.AsNumber()].front();
			if (c) return std::static_pointer_cast<T>(c);
		}
		return std::shared_ptr<T>();
	}

	template<typename T>
	inline std::vector<std::shared_ptr<T>> Node::GetComponents() {
		static BitMask id = TypeInfo::GetComponentID<T>();
		std::vector<std::shared_ptr<T>> toReturn = std::vector<std::shared_ptr<T>>();
		if (components.count(id.AsNumber()) > 0) {
			std::vector<std::shared_ptr<Component>> c = components[id.AsNumber()];
			for (std::vector<std::shared_ptr<Component>>::iterator i = c.begin(); i != c.end(); ++i) {
				toReturn.push_back(std::static_pointer_cast<T>(*i));
			}
		}
		return toReturn;
	}

	template<typename T>
	inline std::shared_ptr<T> Node::AddComponent(const std::shared_ptr<T> component) {
		static BitMask id = TypeInfo::GetComponentID<T>();
		std::shared_ptr<Node> me = std::static_pointer_cast<Node>(shared_from_this());
		
		components[id.AsNumber()].push_back(std::static_pointer_cast<Component>(component));
		component->SetOwner(me);
		bitmask |= id;
		if (!componentsDirty) componentsDirty = true;
		return component;
	}
	template<typename T>
	inline std::shared_ptr<T> Node::AddComponent() {
		std::shared_ptr<T> t = std::make_shared<T>();
		return AddComponent(t);
	}
	template<typename T>
	inline const bool Node::HasComponent() const {
		return HasAnyOfComponents(TypeInfo::GetComponentID<T>());
	}
	struct FindNodeById {
		FindNodeById(const std::uint32_t & toFind) : id(toFind) {}
		FindNodeById(const std::shared_ptr<Node> & toFind) : id(toFind->GetID()) {}
		FindNodeById(const std::weak_ptr<Node> & toFind) : id(toFind.lock()->GetID()) {}
		bool operator()(const std::shared_ptr<Node> & other) { return other && other->GetID() == id; }
		bool operator()(const std::weak_ptr<Node> & other) { return other.use_count() > 0  && other.lock()->GetID() == id; }
		std::uint32_t id;
	};
}
#endif // !UNNAMED_NODE_H
