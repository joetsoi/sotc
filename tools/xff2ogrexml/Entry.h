#ifndef Entry_h
#define Entry_h
#include <stdint.h>
#include <iostream>
#include <OgreVector4.h>
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


	};

	/*! \brief pretty print an Entry in a nice human readable format
	 *
	 * print in a human readable way, i'm just using this for debugging
	 * at the moment, but it's good if i want to explore the format further
	 * later
	 */
	std::ostream &operator<<(std::ostream &stream, Entry entry);

	/*! \brief read an entry form a binary stream
	 *
	 * syntactic sugar, constantly typing reinterpret casting from ifstream.read
	 * is not only annoying but it's an eyesore
	 */
	std::istream &operator>>(std::istream &stream, Entry &entry);


	struct Fixed16{
		int16_t x, y , z, w;
		inline Ogre::Vector4 getVector() const { return Ogre::Vector4(
			x * 0.000244140625,
			y * 0.000244140625,
			z * 0.000244140625,
			w * 0.000244140625);
		};
	};

	struct Colour{
		uint8_t r, g, b, a;
	};

	std::ostream& operator<<(std::ostream &stream, Colour &c);

	struct TextureMap{
		uint16_t u, v;
	};

	std::ostream& operator<<(std::ostream &stream, TextureMap &t);

	struct VertexWeight{
		float boneA;
		float boneB;
		float boneC;
		uint32_t numberOfBones;
	};

	std::ostream& operator<<(std::ostream &stream, VertexWeight &w);
	struct StartState{
		uint8_t a, b;
		uint16_t c;
		uint32_t d, e, f;
	};

	std::ostream &operator<<(std::ostream &stream, StartState state);
}

#endif
