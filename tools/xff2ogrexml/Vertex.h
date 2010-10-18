#ifndef Vertex_h
#define Vertex_h
#include <iostream>
#include <OgreVector3.h>
#include <OgreVector2.h>
#include "Entry.h"
namespace sotc {

	class Vertex{
		public:
			Vertex(const Ogre::Vector3 &position = Ogre::Vector3::ZERO)  
				: hasNormal(false), hasTexture(false)
				, hasTexture2(false), hasBones(false), position(position){}
			uint32_t getSurface() const { return surface; }
			void setNormal(const Ogre::Vector3 &normal);
			inline Ogre::Vector3 const& getPosition() const{ return position; }
			inline Ogre::Vector3 const& getNormal() const{ return normal; }
			inline Colour const& getColour() const{ return colour; }
			inline Ogre::Vector2 const& getUvMap() const { return textureCoordinates; }
			inline Ogre::Vector2 const& getUvMap2() const { return textureCoordinates2; }
			inline VertexWeight const& getVertexWeight() const { return vertexWeight; }


			void setBoneWeight(const VertexWeight &vertexWeight);
			void setColour(const Colour &colour);
			void setUvMap(const Ogre::Vector2 &uv);
			void setUvMap2(const Ogre::Vector2 &uv);

			bool hasNormal;
			bool hasTexture;
			bool hasTexture2;
			bool hasBones;
		private:
			Ogre::Vector3 position;
			Ogre::Vector3 normal;
			Colour colour;
			Ogre::Vector2 textureCoordinates;
			Ogre::Vector2 textureCoordinates2;
			VertexWeight vertexWeight;
			uint32_t surface;
	};

	bool operator==(const Vertex &lhs, const Vertex &rhs);
	bool operator< (const Vertex &lhs, const Vertex &rhs);
	std::ostream &operator<<(std::ostream &stream, Vertex vertex);
}
#endif
