/*! based of code at http://repo.or.cz/dormin.git
 */
#ifndef Xff_h
#define Xff_h
#include <iostream>
#include <fstream>
#include <vector>

#include "Surface.h"
#include "Texture.h"
#include "SectionHeader.h"
namespace sotc {
	template <typename T>
		T readObject(std::ifstream &xff){
			T t;
			xff.read(reinterpret_cast<char*>(&t), sizeof(T));
			return t;
		}

	class Xff{
		private:
			int offset;
			int rodataAddress;
			int rodataSize;
			SectionHeaders headers;
			std::vector<TextureHeader> textureHeaders;
			std::vector<SurfaceHeader> surfaceHeaders;
			std::vector<GeometryHeader> geometryHeaders;
			std::vector<Surface> surfaces;

/*
			enum Type{
				UVMAP = 0x65,
				VERTEX =  0x68,
				VERTEXWEIGHT = 0x6c,
				NORMALTANGENT16BIT = 0x6d,
				COLOUR = 0x6e

			};
	*/
		public:
			Xff(const char *filename);
			void readLocations(std::ifstream &xff);
			void readHeaders(std::ifstream &xff);
			void readVertices(std::ifstream &xff);

			std::vector<Surface>& getSurfaces() { return surfaces; }
	}; 
}
#endif
