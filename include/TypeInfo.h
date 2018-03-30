#ifndef UNNAMED_TYPE_INFO_H
#define UNNAMED_TYPE_INFO_H
#include "Object.h"
#include "BitMask.h"
#include <unordered_map>
namespace B00289996 {
	template<typename T>
	class ObjectContainer {
	public:
		ObjectContainer(const T & object) : o(object) {}
		ObjectContainer(const ObjectContainer& toCopy) : o(toCopy.o) {}
		ObjectContainer(const ObjectContainer&& toCopy) : o(toCopy.o) {}
		~ObjectContainer() {}
		T GetObject() { return o; }
	private:
		T o;
	};
	struct RenderHandle {
		const bool IsValid() const { return m_value; }
		const std::size_t GetValue() const { return m_value; }
		const std::string GetResourceType() const { return m_type; }
	private:
		RenderHandle(const std::string & type, std::size_t value) : m_type(type), m_value(value) {
		}
		std::string m_type = "";
		std::size_t m_value = 0;
	};
	class TypeInfo {
	public:
		template <typename T>
		static const std::size_t GetID() {
			static std::size_t id = startID++;
			return id;
		}

		template <typename T = std::enable_if<std::is_base_of<Component, T>::value>::type>
		static const BitMask GetComponentID() {
			static const BitMask id = BitMask().Set(componentCount++);
			return id;
		}
		
	private:
		static std::size_t startID;
		static std::size_t componentCount;
	};
}
#endif // !UNNAMED_TYPE_INFO_H
