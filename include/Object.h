#ifndef UNNAMED_OBJECT_H
#define UNNAMED_OBJECT_H
#include "TypeInfo.h"
#include "Emitter.h"
#include <string>
#include <memory>
#include <vector>
namespace B00289996 {

	class SGObject : public ConnectionMananger, public std::enable_shared_from_this<SGObject> {
	public:
		SGObject();
		~SGObject();
	protected:
		std::size_t objectID;
	};

	
}
#endif // !UNNAMED_OBJECT_H

