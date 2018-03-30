#include "Texture.h"
#include "IDGenerator.h"
namespace B00289996 {

	Texture::Texture() : std::enable_shared_from_this<Texture>(), RenderableResource(), filePath(""), mtype(TextureType::DIFFUSE), width(0), height(0), initialized(true), textureData(nullptr){
		id = IDGenerator<Texture, uint16_t>::GetInstance().GetNewID();
	}
	Texture::~Texture() {
		IDGenerator<Texture, uint16_t>::GetInstance().RetireID(id);
	}
	const TextureType Texture::GetType() const {
		return mtype;
	}
	void Texture::SetType(TextureType type) {
		if (mtype != type) {
			mtype = type;
		} 
	}
	const std::uint32_t Texture::GetDataSize() const {
		return dataSize;
	}
}
