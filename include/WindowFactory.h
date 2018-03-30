#ifndef UNNAMED_WINDOW_FACTORY_H
#define UNNAMED_WINDOW_FACTORY_H
#include "Factory.h"
#include "ObjectCreationInfo.h"
#include <memory>
namespace B00289996 {
	class Window;
	class WindowFactory : public IFactory<Window, WindowCreationInfo> {
	public:
		WindowFactory();
		std::shared_ptr<Window> CreateShared(const WindowCreationInfo & params) override;
		std::unique_ptr<Window> CreateUnique(const WindowCreationInfo & params) override;
	};
}
#endif // !UNNAMED_WINDOW_FACTORY_H
