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
			//Ogre::Vector3 tangent;	//if not all vertices have tgts should i set as ptr?
			VertexWeight vertexWeight; //same as above
			Colour colour;
			TextureMap textureMap;
			uint32_t surface;
		public:
			Vertex(const Ogre::Vector3 &position = Ogre::Vector3::ZERO) : position(position){
				hasNormal = false;
				hasTexture = false;
				hasBones = false;
			}
			uint32_t getSurface() const { return surface; }
			void setNormal(const Ogre::Vector3 &normal);
			Ogre::Vector3 const& getPosition() const{ return position; }
			Ogre::Vector3 const& getNormal() const{ return normal; }
			Colour const& getColour() const{ return colour; }
			TextureMap const& getUvMap() const { return textureMap; }

			void setBoneWeight(const VertexWeight &vertexWeight);
			void setColour(const Colour &colour);
			void setUvMap(const TextureMap &textureMap);
			//void setUvMap(const Ogre::Vector3 &uvwMap);

			bool hasNormal;
			bool hasTexture;
			bool hasBones;
	};

	//!
	bool operator< (const Vertex &lhs, const Vertex &rhs);
	std::ostream &operator<<(std::ostream &stream, Vertex vertex);
}
#endif
