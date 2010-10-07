#ifndef Surface_h
#define Surface_h
#include <stdint.h>
#include <boost/scoped_array.hpp>
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
			Surface() {};
			//Surface(const SurfaceHeader &surfaceHeader);
			
			void addStrip(const std::vector<Vertex> &strip) { strips.push_back(strip); }
			//void addStrip(const std::vector<Vertex> &strip) { strips.push_back(strip); }
			
			
			void constructTriangleList();

			//const std::vector<Strip>& getStrips() const { return strips; }
			const std::vector<Triangle> getTriangles() const { return faces; }
			//const std::vector<Vertex>& getVertices() const;
			//int getTotalVertices () const { return totalVertices; }
			
			const UniqueMap& getUniqueMap() const { return vertexMap; }
			/*
			void uniqueVertices();

			const std::vector<Triple>& getTriangles() const {  return faces; } //make mutable?
			*/
		private:
			void constructUniqueVertices();
			//void createTrianglesFromStrip(const std::vector<Vertex> &strip);
			std::vector<std::vector<int> > indexedVertices;
			std::vector<std::vector<Vertex> > strips;

			UniqueMap vertexMap;
			std::vector<Triangle> faces;

			/*


			//int vertexCount;
			*/

	};
}
#endif
