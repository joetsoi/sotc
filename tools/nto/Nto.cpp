#include "Nto.h"
#include <stdexcept>
#include <cassert>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace sotc;

class ParseException : public std::runtime_error {
	public:
		ParseException(const char *msg = "") : std::runtime_error(msg){}

};



/*! report the expected and actual results of a magic number
*/
class BadMagicException : public ParseException {
	public:
		BadMagicException(const char *expected, const char *actual)
			: ParseException("Bad Magic Exception")
			  , expected(expected), actual(actual){}
		const char *expected;
		const char *actual;
};



Nto::Nto(const std::string &filename) : filename(filename) {
	xff.open(filename.c_str(), std::ios::in|std::ios::binary);
	if(xff){
		readLocations();
		xff.seekg(rodataAddress + offset, std::ios::beg);
		try{
			readAndCheckMagic4("NTO2");
		} catch(BadMagicException e){
			std::cerr << e.what() << " expected: " << e.expected << " got: "
				<< e.actual << " at 0x" << std::hex << xff.tellg() <<std::endl;
		}
		xff.ignore(16);
		xff.read(reinterpret_cast<char*>(&header), sizeof(header));
		width = 1 << (header.wh & 0xf);
		height = 1 << (header.wh >> 4);
		std::cout << "dim " << width << " " << height << " pix";
		std::cout << header.pixelsAddress << " pal" << header.paletteAddress
			<< " t:" << (uint32_t) header.type << " s:" << (uint32_t)header.swizzle <<'\n';
		convertRgba();
	} else {
		std::cerr << "error opening file" << std::endl;
	}

}



void Nto::readLocations(){
	//const int NUMBER_OF_SECTIONS = 0x40;
	const int OFFSET = 0x4c;//distance of section headers from beginning of file;
	const int RODATA_SIZE = 0xa0;
	const int RODATA_ADDRESS = 0xb4;

	//number of sections in this file, don't actually think I need it.
	//so i've commented it out.
	//xff.seekg(NUMBER_OF_SECTIONS, std::ios::beg);
	//xff.read(reinterpret_cast<char*>(&sectionCount), sizeof(int));
	xff.seekg(OFFSET, std::ios::beg); 
	xff.read(reinterpret_cast<char*>(&offset), sizeof(int));
	xff.seekg(RODATA_SIZE, std::ios::beg);
	xff.read(reinterpret_cast<char*>(&rodataSize), sizeof(int));
	xff.seekg(RODATA_ADDRESS, std::ios::beg);	//address of rodata segment
	xff.read(reinterpret_cast<char*>(&rodataAddress), sizeof(int));
}



void Nto::readAndCheckMagic4(const std::string &expected){
	char magic[5];
	xff.read(magic, 4);
	magic[4] = '\0';
	std::string smagic(magic);
	//std::cout << std::string(magic) << '\n';
	//perhaps substring expected down to 3 characters?
	//doesn't account for length expected > length magic eg. if expected=xffMAGIC
	//it would throw the exception, i figured it shouldn't make a huge difference
	if(expected.compare(smagic) != 0){
		throw BadMagicException(expected.c_str(), magic);
	}
}



void Nto::convertRgba(){
	rgba.resize(width * height);
	xff.seekg(header.pixelsAddress + rodataAddress);
	switch(header.type){
		case 0x00://32bit
			xff.read(reinterpret_cast<char*>(&rgba[0]), sizeof(rgba));
			break;
		case 0x14:
		{//4bit

			//assert(xff.tellg() == header.paletteAddress);
			if(!header.swizzle){
				std::vector<char> pixels;
				pixels.resize(width * height / 2);
				xff.seekg(header.pixelsAddress + rodataAddress);
				xff.read(reinterpret_cast<char*>(&pixels[0]), pixels.size());

				//assert(header.paletteAddress + rodataAddress == xff.tellg());
				// assert only applies when there are no mipmaps, i'm not reading
				// them at the moment
				//
				std::vector<RGBQUAD> palettes;
				palettes.resize(16); //4bit texture therefore 16 colour palette
				xff.seekg(header.paletteAddress + rodataAddress);
				xff.read(reinterpret_cast<char*>(&palettes[0]), 16 * sizeof(RGBQUAD));

								uint32_t i = 0;
				foreach(char pixelduo, pixels){
					rgba[i] = palettes[pixelduo & 0xf];
					rgba[i+1] = palettes[(pixelduo & 0xf0)>>4];
					i = i + 2;
				}
			} else {
				unswizzle32to4();
			}
			break;
		}
		case 0x13:
		{
			if(!header.swizzle){
				xff.seekg(header.pixelsAddress + rodataAddress);
				std::vector<uint8_t> pixels;
				pixels.resize(width * height);
				xff.read(reinterpret_cast<char*>(&pixels[0]), pixels.size());

				//same as above this assert condition does not apply due to not
				//reading mipmaps
				//assert(header.paletteAddress + rodataAddress == xff.tellg());
				std::vector<RGBQUAD> palettes;
				palettes.resize(256);//8 bit texture therefore 256 colours
				xff.seekg(header.paletteAddress + rodataAddress);
				xff.read(reinterpret_cast<char*>(&palettes[0]), 256 * sizeof(RGBQUAD));
				int i = 0;
				foreach(uint8_t pixel, pixels){
					rgba[i] = palettes.at(pixel);
					i++;
				}
			} else {
				unswizzle32to8();
			}
		}
		break;
	}

	saveImage(width, height);
}



void Nto::unswizzle32to8(){
	std::vector<RGBQUAD> palettes;
	palettes.resize(256);//8 bit texture therefore 256 colours
	xff.seekg(header.paletteAddress + rodataAddress);
	xff.read(reinterpret_cast<char*>(&palettes[0]), 256 * sizeof(RGBQUAD));

	std::vector<uint8_t> source;
	source.resize(width * height);
	xff.seekg(rodataAddress + header.pixelsAddress);
	xff.read(reinterpret_cast<char*>(&source[0]), source.size());

	int i = 0;
	for(uint32_t y = 0; y < height; ++y){
		for(uint32_t x = 0; x < width; ++x){
			const uint32_t blockLocation = (y & (~0xf)) * width + (x & (~0xf)) * 2;
			const uint32_t swapSelector = (((y + 2) >> 2) & 0x1) * 4;
			const uint32_t yPosition = (((y & (~3)) >> 1) + (y & 1)) & 0x7;
			const uint32_t columnLocation = yPosition * width * 2 + ((x + swapSelector) & 0x7) * 4;
			const uint32_t byteNumber = ((y >> 1) & 1) + ((x >> 2) & 2); //1,2,3?
			//std::cout << blockLocation + columnLocation + byteNumber << '\n';
			rgba[i] = palettes.at(source.at(blockLocation + columnLocation + byteNumber));
			++i;
		}
	}

}



void Nto::unswizzle32to4(){
	std::vector<RGBQUAD> palettes;
	palettes.resize(16); //4bit texture therefore 16 colour palette
	xff.seekg(header.paletteAddress + rodataAddress);
	xff.read(reinterpret_cast<char*>(&palettes[0]), 16 * sizeof(RGBQUAD));

	std::vector<uint8_t> source;
	source.resize(width * height);
	xff.seekg(header.pixelsAddress + rodataAddress);
	xff.read(reinterpret_cast<char*>(&source[0]), source.size());

	int i = 0;
	for(uint32_t y = 0; y < height; y++){
		for(uint32_t x = 0; x < width; ++x){
			const uint32_t pageX = x & (~0x7f);
			const uint32_t pageY = y & (~0x7f);

			const uint32_t horizontalPages = (width + 127) / 128;
			const uint32_t verticalPages = (height + 127) / 128;

			const uint32_t pageNumber = (pageY/128) * horizontalPages + (pageX / 128);

			const uint32_t page32y = (pageNumber / verticalPages) * 32;
			const uint32_t page32x = (pageNumber % verticalPages) * 64;

			const uint32_t pageLocation = page32y * height * 2 + page32x * 4;

			const uint32_t yLocation = y & 0x7f;
			const uint32_t xLocation = x & 0x7f;

			const uint32_t blockLocation = ((xLocation & (~0x1f)) >> 1) * height + (yLocation & (~0xf)) * 2;
			const uint32_t swapSelector = (((y + 2) >> 2) & 0x1) * 4;
			const uint32_t yPosition = (((y & (~3)) >> 1) + (y & 1)) & 0x7;

			const uint32_t columnLocation = yPosition * height * 2 + ((x + swapSelector) & 0x7) * 4;

			const uint32_t byteNumber = (x >> 3) & 3; //0, 1, 2, 3
			const uint32_t bitSet = (y >> 1) & 1;

			uint8_t v = source[pageLocation + blockLocation + columnLocation + byteNumber];
			v = (v >> (bitSet * 4)) & 0x0f;
			rgba[i] = palettes[v];
			i++;
		}
	}
}



void Nto::saveImage(uint32_t width, uint32_t height){
	FreeImage_Initialise();
	FIBITMAP *texture = FreeImage_Allocate(width, height, 32
			//	, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK
			//	, FI_RGBA_BLUE_MASK//, FI_RGBA_ALPHA_MASK
			);

	if(texture){
		//RGBQUAD colour = { 255, 255 , 0, 255 };
		for(uint32_t y = 0; y < height; ++y){
			//BYTE *bits = FreeImage_GetScanLine(texture, y);
			for(uint32_t x = 0; x < width; ++x){
				RGBQUAD unswiz = rgba.at(y * width + x);
				RGBQUAD swiz = { unswiz.rgbRed, unswiz.rgbGreen,  unswiz.rgbBlue, unswiz.rgbReserved };
				FreeImage_SetPixelColor(texture, x, (height - 1) - y, &swiz);
				//FreeImage_SetPixelColor(texture, x, y, &colour);
			}
		}

		//find last / for just the file name and change .nto to .png
		
		std::string outfile = filename.substr(filename.rfind("/")+1);
		outfile.replace(outfile.rfind(".nto"), 4,".png");
		std::cout << outfile << '\n';

		FreeImage_Save(FIF_PNG, texture, outfile.c_str());
		FreeImage_Unload(texture);
	}
	FreeImage_DeInitialise();
}



int main(int argc, char* argv[]){
	if(argc > 1){
		for(int i = 1; i < argc; i++){
			std::cout << argv[i] << '\n';
			Nto nto(argv[i]);
		}
	}else{
		Nto nto(argv[1]);
		return 0;
	}
}
