#ifndef UNNAMED_TEXTURE_LOADER_H
#define UNNAMED_TEXTURE_LOADER_H

#include "Texture.h"

namespace B00289996 {
	class TextureLoader {
	public:
		TextureLoader() {}
		/// <summary>Finalizes an instance of the <see cref="TextureLoader"/> class.</summary>
		~TextureLoader() {}
		virtual void GetTextureData(const std::string & filePath, unsigned char ** dataStorage, std::int32_t & width, std::int32_t & height, std::int32_t & channels, std::uint32_t & size) const = 0;
		virtual void DeleteTextureData(void * data) const = 0;
	};
}

#endif // !UNNAMED_TEXTURE_LOADER_H
