#ifndef Vertex_h
#define Vertex_h
#include <iostream>
#include <OgreVector3.h>
#include "Entry.h"
namespace sotc {

	class Vertex{
		private:
			Ogre::Vector3 position;
			Ogre::Vector3 normal;
			Ogre::Vector3 tangent;	//if not all vertices have tgts should i set as ptr?
			VertexWeight vertexWeight; //same as above
			Colour colour;
			TextureMap textureMap;
			uint32_t surface;
		public:
			Vertex(const Ogre::Vector3 &position, uint32_t surface);
			uint32_t getSurface() const { return surface; }
			void setNormal(const Ogre::Vector3 &normal);
			Ogre::Vector3 const& getPosition() const{ return position; }
			void setTangent(const Ogre::Vector3 &tangent);
			void setBoneWeight(const VertexWeight &vertexWeight);
			void setColour(const Colour &colour);
			void setUvMap(const TextureMap &textureMap);
			TextureMap const& getUvMap() const { return textureMap; }
	};

	//!
	bool operator< (const Vertex &lhs, const Vertex &rhs);
	std::ostream &operator<<(std::ostream &stream, Vertex vertex);
}
#endif
