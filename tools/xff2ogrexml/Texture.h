#ifndef Texture_h
#define Texture_h
#include <stdint.h>
namespace sotc {
	struct TextureHeader{
		char magicNumber[4];
		uint32_t addressOfName;
		uint32_t unknown[4];
		uint16_t a, b, width, height;
	};
}
#endif
