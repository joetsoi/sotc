#ifndef Entry_h
#define Entry_h
#include <stdint.h>
namespace sotc {
	struct Entry{
		uint8_t index;
		uint8_t mask;
		uint8_t count;
		uint8_t type;

		enum Type {
			UVMAP = 0x65,
			VERTEX =  0x68,
			VERTEXWEIGHT = 0x6c,
			NORMALTANGENT16BIT = 0x6d,
			COLOUR = 0x6e
		};

	};

	struct Colour{
		uint8_t r, g, b, a;
	};

	struct TextureMap{
		uint16_t u, v;
	};

	struct VertexWeight{
		float boneA;
		float boneB;
		float boneC;
		uint32_t numberOfBones;
	};

	struct StartState{
		uint8_t a, b;
		uint16_t c;
		uint32_t d, e, f;
	};

}
#endif
