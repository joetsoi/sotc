#ifndef Vertex_h
#define Vertex_h
#include <iostream>
#include <OgreVector3.h>
#include <OgreVector2.h>
#include "Entry.h"
namespace sotc {

	class Vertex{
		public:
			Vertex(const Ogre::Vector3 &position = Ogre::Vector3::ZERO) : position(position){
				hasNormal = false;
				hasTexture = false;
				hasBones = false;
				hasTexture = false;
				hasTexture32 = false;
			}
			uint32_t getSurface() const { return surface; }
			void setNormal(const Ogre::Vector3 &normal);
			inline Ogre::Vector3 const& getPosition() const{ return position; }
			inline Ogre::Vector3 const& getNormal() const{ return normal; }
			inline Colour const& getColour() const{ return colour; }
			inline TextureMap const& getUvMap() const { return textureMap; }
			inline Ogre::Vector2 const& getTextureCoordinates() const { return textureCoordinates; }
			inline VertexWeight const& getVertexWeight() const { return vertexWeight; }


			void setBoneWeight(const VertexWeight &vertexWeight);
			void setColour(const Colour &colour);
			void setUvMap(const TextureMap &textureMap);
			void setUvMap(const Ogre::Vector2 &uv);
			//void setUvMap(const Ogre::Vector3 &uvwMap);

			bool hasNormal;
			bool hasTexture;
			bool hasTexture32;
			bool hasBones;
		private:
			Ogre::Vector3 position;
			Ogre::Vector3 normal;
			Ogre::Vector2 textureCoordinates;
			VertexWeight vertexWeight; //same as above
			Colour colour;
			TextureMap textureMap;
			uint32_t surface;
	};

	//!
	bool operator==(const Vertex &lhs, const Vertex &rhs);
	bool operator< (const Vertex &lhs, const Vertex &rhs);
	std::ostream &operator<<(std::ostream &stream, Vertex vertex);
}
#endif
