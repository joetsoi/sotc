/*! based of code at http://repo.or.cz/dormin.git
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

			std::map<int, Surface>& getSurfaces() { return surfaces; }
			std::string filename;

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

			/*! \brief reads all the vertices and their attributes
			 *
			 * extracts all the data using the geometry headers a strip at a time
			 * the very first byte indicates how many attributes this set of strips
			 * has. Each strip can be made out of the attributes below
			 *
			 * a strip header
			 * position
			 * normal
			 * uvtexture	(optional)
			 * colour
			 * bone/vertex assignments	(optional)
			 * special (optional) only seen in dormin2wander.nmo and wander2dormin.nmo)
			 *
			 * the strip consists of a list of vertices, followed by a list of normals
			 * etc etc
			 */
			void parseVertices();

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
			void parseStripHeader();


			/*! \brief parse positions
			 *
			 * entry defined by 4 bytes 
			 * 0x01 always the first attribute
			 * 0x80 mask
			 * 0xXX count number of entries
			 * 0x68 always a 32bit floating point
			 *
			 * followed by the 3 floating points
			 */
			std::vector<Ogre::Vector3> parsePosition();

			/*! \brief parse normals
			 *
			 * slightly more complicated than positioning
			 *
			 * entry 
			 * 0x02 optional but will always be 2 if it exists
			 * 0x80 mask
			 * 0xXX number of entires
			 * 0x68 or 0x6d 32 bit or 16 bit fixed point
			 *
			 * floating points can be 32bit floating point or fixed point 16bit
			 * where the resulting float is hte value / 4096
			 *
			 * if the float is 16 bit the last  value is ignored
			 */
			std::vector<Ogre::Vector3> parseNormal(const Entry &entry);
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
			std::map<int, Surface> surfaces;

			std::ifstream xff;

			void stateParse();
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
