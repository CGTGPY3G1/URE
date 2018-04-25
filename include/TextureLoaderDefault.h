#ifndef UNNAMED_TEXTURE_LOADER_DEFAULT_H
#define UNNAMED_TEXTURE_LOADER_DEFAULT_H

#include "TextureLoader.h"
#include <string>
namespace B00289996 {
	class TextureLoaderDefault : public TextureLoader {
	public:
		TextureLoaderDefault();
		/// <summary>Finalizes an instance of the <see cref="TextureLoader"/> class.</summary>
		~TextureLoaderDefault() {}
		virtual void GetTextureData(const std::string & filePath, unsigned char ** dataStorage, std::int32_t & width, std::int32_t & height, std::int32_t & channels, std::uint32_t & size) const override;
		virtual void DeleteTextureData(void * data) const override;
	};
}

#endif // !UNNAMED_TEXTURE_LOADER_DEFAULT_H
