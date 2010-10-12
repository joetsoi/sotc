/*! based of code at http://repo.or.cz/dormin.git many thanks malc!
 */
#ifndef Xff_h
#define Xff_h
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "Surface.h"
#include "Texture.h"
#include "SectionHeader.h"
namespace Ogre{
	class Vector3;
}
namespace sotc {
	struct Entry;
	class State;
	class Xff{
		public:
			//Xff(std::ifstream &xff);
			Xff(const std::string &filename);

			std::map<uint32_t, Surface>& getSurfaces() { return surfaces; }
			std::vector<std::string>& getTextures() { return textures; }
			std::string filename;
			std::string basename();
			std::string withoutExtension();
			//std::string dirname();

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

			/*! \brief reads data about all the strips in this geometry
			 *
			 * the first is the number of 'attributes' each strip has, the rest
			 * i'm yet to figure out, but some seem constant throughout file
			 */
			int parseGeometryDataHeader(const Entry &entry);

			/*! \brief header at the beginning of each strip
			 *
			 * can also appear again on it's own, see parseDouche. I'm not
			 * sure what these do. one variable contains how many vertices
			 * in this strip, it's also duplicated for each attribute
			 */
			void readNames(std::vector<TextureHeader> textureHeaders
							, std::vector<SurfaceHeader> surfaceHeaders);

			void stateParse(std::vector<GeometryHeader> geometryHeaders);

			int offset;
			int rodataAddress;
			int rodataSize;
			std::map<uint32_t, Surface> surfaces;
			std::vector<std::string> textures;

			std::ifstream xff;

			State runStart(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices);
			State runGetPosition(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices);
			State runGetNormal(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices);
			State runGetTexture(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices);
			State runGetColour(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices);
			State runGetBones(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices);

			Entry last;
			State runFinishStrip(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices);
	}; 
}
#endif
