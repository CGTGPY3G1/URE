#ifndef UNNAMED_FACTORY_H
#define UNNAMED_FACTORY_H
#include <vector>
#include <string>
#include <memory>

namespace B00289996 {
	template <typename Type, typename CreateInfo = std::vector<std::string>> class IFactory {
	public:
		IFactory() {}
		virtual std::shared_ptr<Type> CreateShared(const CreateInfo & params) = 0;
		virtual std::unique_ptr<Type> CreateUnique(const CreateInfo & params) = 0;
	};
}
#endif // !UNNAMED_FACTORY_H
