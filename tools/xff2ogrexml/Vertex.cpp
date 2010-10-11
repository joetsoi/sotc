#include "Vertex.h"
namespace sotc {
	void Vertex::setNormal(const Ogre::Vector3 &normal){
		this->normal = normal;
		hasNormal = true;
	}
//	void Vertex::setTangent(const Ogre::Vector3 &tangent){
//		this->tangent = tangent;
//	}

	void Vertex::setBoneWeight(const VertexWeight &vertexWeight){
		this->vertexWeight = vertexWeight;
		hasBones = true;
	}

	void Vertex::setColour(const Colour &colour){
		this->colour = colour;
	}

	void Vertex::setUvMap(const TextureMap &textureMap){
		this->textureMap = textureMap;
		hasTexture = true;
	}



	bool operator==(const Vertex& lhs, const Vertex &rhs){
		if(lhs.getPosition() == rhs.getPosition()
			&& lhs.getNormal() == rhs.getNormal()
			&& lhs.getColour() == rhs.getColour()
			&& lhs.getUvMap() == rhs.getUvMap()
			&& lhs.getVertexWeight() == rhs.getVertexWeight()){

			return true;
		} else {
			return false;
		}
	}



	bool operator< (const Vertex& lhs, const Vertex &rhs){
		if(lhs.getPosition().x < rhs.getPosition().x){
			return true;
		} else if(lhs.getPosition().x == rhs.getPosition().x) {
			if(lhs.getPosition().y < rhs.getPosition().y){
				return true;
			} else if(lhs.getPosition().y == rhs.getPosition().y) {
				if(lhs.getPosition().z < rhs.getPosition().z){
					return true;
				}
			}
		}
		return false;	
	}

	std::ostream &operator<<(std::ostream &stream, Vertex vertex){
		stream << vertex.getPosition();
		return stream;
	}
}
