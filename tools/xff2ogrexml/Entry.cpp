#include "Entry.h"

namespace sotc{

	std::ostream &operator<<(std::ostream &stream, Entry entry){
		stream << "i: " << (int)entry.index << " m: " << (int)entry.mask
			<< " #: " << (int)entry.count << " t: " << (int)entry.type;
		return stream;
	}



	std::istream &operator>>(std::istream &stream, Entry &entry){
		stream.read(reinterpret_cast<char*>(&entry), sizeof(Entry));
		return stream;
	}



	std::ostream &operator<<(std::ostream &stream, StartState state){
		stream << "StartState " << (int)state.a << " " << (int)state.b << " " << (int)state.c
			<< " " << (int)state.d << " " << (int)state.e << " " << (int)state.f;
		return stream;
	}

	std::ostream& operator<<(std::ostream &stream, Colour &c){
		stream << "Colour " << (int)c.r << " " << (int)c.g << " " << (int)c.b << " " << (int)c.a;
		return stream;
	}

	std::ostream& operator<<(std::ostream &stream, TextureMap &t){
		stream << "texture " << t.u << " " << t.v;
		return stream;
	}
	
	std::ostream& operator<<(std::ostream &stream, VertexWeight &t){
		stream << "bone " << t.boneA << " " << t.boneB << " "<< t.boneC << " " << t.numberOfBones;
		return stream;
	}
	
}
