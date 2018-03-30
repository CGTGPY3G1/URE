#ifndef UNNAMED_TEXTURE_H
#define UNNAMED_TEXTURE_H
#include "RenderableResource.h"
#include "MessagingSystem.h"
#include <memory>
#include <string>
namespace B00289996 {
	enum class TextureType : std::uint32_t {
		DIFFUSE = 0,
		NORMAL_MAP = 1,
	};
	class Texture : public std::enable_shared_from_this<Texture>, public RenderableResource {
		friend struct TextureIsType;
		friend class TextureLoader;
		friend class FileInput;
		
	public:
		Texture();
		~Texture();
		const bool operator==(const Texture & other) { return filePath.compare(other.filePath) == 0; }
		const bool operator!=(const Texture & other) { return !(*this == other); }
		const std::string GetFilePath() const {
			return filePath;
		}
		const std::uint16_t GetID() const {
			return id;
		}
		const std::size_t GetWidth() const {
			return width;
		}
		const std::size_t GetHeight() const {
			return height;
		}

		const std::size_t GetChannels() const {
			return channels;
		}
		unsigned char * GetTextureData() {
			return textureData;
		}
		const TextureType GetType() const;
		void SetType(TextureType type);

		const std::uint32_t GetDataSize() const;
	private:
		void SetTextureData(unsigned char * data) {
			if (textureData != nullptr) delete textureData;
			textureData = data;
		}
		void DeleteData() {
			if (textureData) {
				delete textureData;
				textureData = nullptr;
			}
		}
		std::size_t width, height, channels;
		std::uint32_t dataSize = 0;
		uint16_t id;
		TextureType mtype;
		std::string filePath;
		bool initialized;
		unsigned char * textureData;
	};
	static const bool operator==(const std::shared_ptr<Texture> & l, const std::shared_ptr<Texture> & r) { return (l && r && l->GetFilePath().compare(r->GetFilePath()) == 0); }
	static const bool operator!=(const std::shared_ptr<Texture> & l, const std::shared_ptr<Texture> & r) { return !(l == r); }
	struct TextureIsType {
		TextureIsType(const TextureType & type) : t(type) {}
		bool operator()(const Texture & other) { return other.mtype == t; }
		bool operator()(const Texture * other) { return other != nullptr && other->mtype == t; }
	private:
		TextureType t;
	};

}
#endif // !UNNAMED_TEXTURE_H

