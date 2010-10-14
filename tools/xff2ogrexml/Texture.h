#ifndef Texture_h
#define Texture_h
#include <stdint.h>
#include <string>
namespace sotc {
	struct TextureHeader{
		char magicNumber[4];
		uint32_t addressOfName;
		uint32_t unknown[4];
		uint16_t textureId, isTransparent, width, height;
	};

	class Texture{
	public:
		Texture(const std::string &name, const int isTransparent)
					: name(name), isTransparent(isTransparent){}
		std::string name;
		int isTransparent;
	};
}
#endif
