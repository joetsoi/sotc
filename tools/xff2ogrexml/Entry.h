#ifndef Entry_h
#define Entry_h
#include <stdint.h>
#include <iostream>
#include <OgreVector3.h>
namespace sotc {
	struct Entry{
		uint8_t index;
		uint8_t mask;
		uint8_t count;
		uint8_t type;

		enum Type {
			UVMAP = 0x65,
			FLOAT32 =  0x68,
			VERTEXWEIGHT = 0x6c,
			FLOAT16 = 0x6d,
			COLOUR = 0x6e
		};

		Entry() : index(0), mask(0), count(0), type(0) {}
		Entry(uint8_t index, uint8_t mask, uint8_t count, uint8_t type)
			: index(index), mask(mask), count(count), type(type) {}
		inline bool operator==(const Entry &r) const{
			return ( index == r.index && mask == r.mask
					&& count == r.count && type == r.type);
		}
		
		inline bool operator!=(const Entry &r) const{
			return !(*this == r);
		}

		/*! \brief pretty print an Entry in a nice human readable format
		 *
		 * print in a human readable way, i'm just using this for debugging
		 * at the moment, but it's good if i want to explore the format further
		 * later
		 */
		inline friend std::ostream &operator<<(std::ostream &stream, Entry entry){
			stream << "i: " << (int)entry.index << " m: " << (int)entry.mask
				<< " #: " << (int)entry.count << " t: " << (int)entry.type;
			return stream;
		}

		/*! \brief read an entry form a binary stream
		 *
		 * syntactic sugar, constantly typing reinterpret casting from ifstream.read
		 * is not only annoying but it's an eyesore
		 */
		inline friend std::istream &operator>>(std::istream &stream, Entry &entry){
			stream.read(reinterpret_cast<char*>(&entry), sizeof(Entry));
			return stream;
		}
	};
	
	struct Fixed16{
		int16_t x, y , z, w;
		inline Ogre::Vector3 getVector() const { return Ogre::Vector3(
			x * 0.000244140625,
			y * 0.000244140625,
			z * 0.000244140625);
		};
	};

	struct Colour{
		inline friend std::ostream& operator<<(std::ostream &stream, Colour &c){
			stream << "Colour " << (int)c.r << " " << (int)c.g << " " << (int)c.b << " " << (int)c.a;
			return stream;
		}
		uint8_t r, g, b, a;
	};

	inline bool operator==(const Colour &lhs, const Colour &rhs){
		if(lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a)
			return true;
		else
			return false;
	}


	struct TextureMap{
		int16_t u, v;

		inline friend std::ostream& operator<<(std::ostream &stream, TextureMap &t){
			stream << "texture " << t.u << " " << t.v;
			return stream;
		}
	};

	inline bool operator==(const TextureMap &lhs, const TextureMap &rhs){
		if(lhs.u == rhs.u && lhs.v == lhs.v)
			return true;
		else
			return false;
	}

	struct VertexWeight{
		float boneA;
		float boneB;
		float boneC;
		uint32_t numberOfBones;
		inline friend std::ostream& operator<<(std::ostream &stream, VertexWeight &t){
			stream << "bone " << t.boneA << " " << t.boneB << " "<< t.boneC << " " << t.numberOfBones;
			return stream;
		}
	};

	inline bool operator==(const VertexWeight &lhs, const VertexWeight &rhs){
		if(lhs.boneA == rhs.boneB && lhs.boneB == rhs.boneB && lhs.boneC == rhs.boneC
			&& lhs.numberOfBones == rhs.numberOfBones){
			return true;
		} else {
			return false;
		}
	}

	struct StartState{
		uint8_t a, b;
		uint16_t c;
		uint32_t d, e, f;

		inline friend std::ostream &operator<<(std::ostream &stream, StartState state){
			stream << "StartState " << (int)state.a << " " << (int)state.b << " " << (int)state.c
				<< " " << (int)state.d << " " << (int)state.e << " " << (int)state.f;
			return stream;

		}
	};

}

#endif
