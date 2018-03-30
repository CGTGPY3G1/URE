#include "Object.h"
#include "IDGenerator.h"
namespace B00289996 {
	SGObject::SGObject() : ConnectionMananger(), std::enable_shared_from_this<SGObject>() {
		objectID = IDGenerator<SGObject>::GetInstance().GetNewID();
	}



	SGObject::~SGObject() {
		IDGenerator<SGObject>::GetInstance().RetireID(objectID);
	}
}
