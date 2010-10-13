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

//	void Vertex::setUvMap(const TextureMap &textureMap){
//		this->textureMap = textureMap;
//		hasTexture = true;
//	}

	void Vertex::setUvMap(const Ogre::Vector2 &vector){
		std::cout << vector << '\n';
		textureCoordinates = vector;
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
		for(int i = 0; i < 3; i++){
			if(lhs.getPosition()[i] == rhs.getPosition()[i])
				continue;
			else if(lhs.getPosition()[i] < rhs.getPosition()[i])
				return true;
			else
				return false;
		}

		for(int i = 0; i < 3; i++){
			if(lhs.getNormal()[i] == rhs.getNormal()[i])
				continue;
			else if(lhs.getNormal()[i] < rhs.getNormal()[i])
				return true;
			else
				return false;
		}

		if(lhs.getUvMap().x < rhs.getUvMap().x){
			return true;
		} else if(lhs.getUvMap().x == rhs.getUvMap().x) {
			if(lhs.getUvMap().y < rhs.getUvMap().y){
				return true;
			} else if(lhs.getVertexWeight() < rhs.getVertexWeight()) {
				return true;
			}
		}

		return false;

	}

	std::ostream &operator<<(std::ostream &stream, Vertex vertex){
		stream << vertex.getPosition();
		return stream;
	}
}
