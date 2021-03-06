#ifndef Surface_h
#define Surface_h
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <boost/tuple/tuple.hpp>
#include "Vertex.h"

namespace sotc {
	struct SurfaceHeader{
		char magicNumber[4];
		uint32_t triangleCount;
		uint32_t stripCount;
		uint32_t  addressOfNames;

		struct Texture{
			uint8_t maybeColour[3];
			float maybeBlend;//assuming don't actually know
			uint32_t unknown;
			uint32_t textureId;
		} textures[3];

		struct Header2{
			uint32_t a;
			float b, c;
			uint32_t d, e, f, g, h;
		} header2;

		uint32_t header3[48];
	}; // __attribute__((__packed__)); // don't appear to need this

	struct GeometryHeader{
		uint32_t size;
		uint32_t unknownA;
		uint32_t surface;
		uint32_t unknownB;
		uint32_t offset;
		uint32_t entryCount;
		uint32_t stripCount;
		uint32_t unknown;
	};

	typedef std::map<Vertex, int> UniqueMap;
	typedef boost::tuple<int, int, int> Triangle;
	class Surface{
		public:
			Surface() {} // could throw an exception! to avoid this map thing
			Surface(const std::string &name, const SurfaceHeader &header);
			
			void addStrip(const std::vector<Vertex> &strip) { strips.push_back(strip); }
			
			void constructTriangleList();

			const std::vector<Triangle> getTriangles() const { return faces; }
			
			uint32_t texture[3];
			const UniqueMap& getUniqueMap() const { return vertexMap; }
			inline void setName(const std::string &s){ name = s; }
			inline int getTriangleCount() const { return triangleCount; }
			inline const std::string& getName() const { return name; }
			inline void setTextures(int one, int two, int three){
				texture[0] = one;
				texture[1] = two;
				texture[3] = three;
			}

		private:
			void constructUniqueVertices();
			std::vector<std::vector<int> > indexedVertices;
			std::vector<std::vector<Vertex> > strips;

			UniqueMap vertexMap;
			std::vector<Triangle> faces;
			std::string name;
			int stripCount;
			int triangleCount;
	};
}
#endif
