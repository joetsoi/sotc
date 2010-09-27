#ifndef SectionHeader_h
#define SectionHeader_h
#include <stdint.h>
namespace sotc {
	struct SectionHeader{
		uint32_t address;
		uint32_t numberOfEntries;
		uint32_t a;
		uint32_t b;
	};

	struct SectionHeaders{
		SectionHeader header0;
		SectionHeader textures;
		SectionHeader surfaces;
		SectionHeader geometry;
	};
}
#endif
