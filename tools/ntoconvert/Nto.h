#ifndef Nto_h
#define Nto_h
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <FreeImage.h>

namespace sotc{
	class Nto{
		public:
			Nto(const std::string &filename);
		private:
			void readLocations();
			void readAndCheckMagic4(const std::string &expected);
			void convertRgba();
			void saveImage(uint32_t width, uint32_t height);
			void unswizzle32to8();
			void unswizzle32to4();

			struct Header{
				uint32_t pixelsAddress;
				uint32_t paletteAddress;
				uint8_t type;
				uint8_t mipmaps;
				uint8_t wh;
				uint8_t swizzle;
			}; 

			std::vector<RGBQUAD> rgba;
			std::string filename;
			std::ifstream xff;
			uint32_t offset;
			uint32_t rodataAddress;
			uint32_t rodataSize;
			Header header;
			uint32_t width, height;
	};
}
#endif
