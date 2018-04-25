#include "TextureLoaderDefault.h"
#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty\stb\stb_image.h"
#include "Texture.h"
#include <iostream>
namespace B00289996 {
	TextureLoaderDefault::TextureLoaderDefault() : TextureLoader() {
	}

	void TextureLoaderDefault::GetTextureData(const std::string & filePath, unsigned char ** dataStorage, std::int32_t & width, std::int32_t & height, std::int32_t & channels, std::uint32_t & size) const {
		*dataStorage = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_default);
		size = width * height * channels;
		if (dataStorage == nullptr) std::cout << stbi_failure_reason() << std::endl;
	}

	void TextureLoaderDefault::DeleteTextureData(void * data) const {
		if(data) stbi_image_free(data);
	}

}
