/*! based of code at http://repo.or.cz/dormin.git
 */
#ifndef Xff_h
#define Xff_h
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Surface.h"
#include "Texture.h"
#include "SectionHeader.h"

namespace sotc {
	struct Entry;
	class Xff{
		public:
			//Xff(std::ifstream &xff);
			Xff(const std::string &filename);

			std::vector<Surface>& getSurfaces() { return surfaces; }

		private:
			/*! \brief read and check a magic number of 4 char length
			 *
			 * reads 4 characters from xff, and compares it to expected
			 * throws an exception if it fails
			 */
			void readAndCheckMagic4(const std::string &expected);

			/*! \brief reads the address of nmo sections rodata etc
			 *
			 * reads and stores the locations of rodata sections used
			 * by the rest of the read* functions
			 *
			 * the offset is where the nmo section starts and where the actual
			 * data i want extracted is stored
			 */
			void readLocations();

			/*! \brief reads headers of the rodata segments
			 *
			 * reads the headers for the surfaces, geometry and texture
			 * tells us how many surfaces there are, where they are located
			 * and how the data is divided.
			 */
			void readHeaders();

			void parseVertices();
			int parseGeometryDataHeader();
			void parseStripHeader();
			void parsePosition();
			void parseNormal();
			void parseColour(int &attributesRemaining);
			void parseTexture();
			void parseVertexWeight();
			void parseDouche();

			void readVertices();


			int offset;
			int rodataAddress;
			int rodataSize;
			SectionHeaders headers;
			std::vector<TextureHeader> textureHeaders;
			std::vector<SurfaceHeader> surfaceHeaders;
			std::vector<GeometryHeader> geometryHeaders;
			std::vector<Surface> surfaces;

			std::ifstream xff;

	}; 
}
#endif
