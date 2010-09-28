#include <exception>
#include <stdint.h>
#include <string.h>
#include <cassert>
#include "Xff.h"
#include <boost/scoped_array.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <OgreVector3.h>
#include <OgreVector4.h>

#include "Entry.h"
#include "Vertex.h"

using namespace sotc;


// my short hand for reading an object because i hate writing reinerpret cast
// replace with stream operator>> as this is just to make the code less clutered
template <typename T>
	T readObject(std::ifstream &xff){
		T t;
		xff.read(reinterpret_cast<char*>(&t), sizeof(T));
		return t;
}



class ParseException : public std::runtime_error {
	public:
		ParseException(const char *msg) : std::runtime_error(msg){}

};



/*! report the expected and actual results of a magic number
*/
class BadMagicException : public ParseException {
	public:
		BadMagicException(const char *expected, const char *actual)
			: ParseException("Bad Magic Exception")
			, expected(expected), actual(actual){}
		const char *expected;
		const char *actual;
};



class MalformedEntry : public ParseException {
public:
	MalformedEntry(int location, Entry entry, const char *error) 
		: ParseException(error)
		, location(location), entry(entry) {}
	
	int location;
	Entry entry;
};



Xff::Xff(const std::string &filename) {
	xff.open(filename.c_str(), std::ios::in|std::ios::binary);
	if(xff){
		try{
			readAndCheckMagic4("xff\x00");
			readLocations();
			readHeaders();
			parseVertices();
		} catch (BadMagicException &e){
			std::cerr << e.what() << " expected: " << e.expected
				<< " got: " << e.actual << std::endl;
		} catch(MalformedEntry &e){
			std::cerr << "Malformed Entry error, terminated: ";
			std::cerr << std::hex << e.what() << " " << e.entry << " "
				 << "0x" << e.location << std::endl;
		}
		xff.close();
	} else {
		std::cerr << "error opening file" << std::endl;
	}
	//readVertices(xff);
}



void Xff::readAndCheckMagic4(const std::string &expected){
	char magic[4];
	xff.read(magic, sizeof(magic));
	//perhaps substring expected down to 3 characters?
	//doesn't account for length expected > length magic eg. if expected=xffMAGIC
	//it would throw the exception, i figured it shouldn't make a huge difference
	if(expected != magic){
		throw BadMagicException(expected.c_str(), magic);
	}
}



void Xff::readLocations(){
	//const int NUMBER_OF_SECTIONS = 0x40;
	const int OFFSET = 0x4c;//distance of section headers from beginning of file;
	const int RODATA_SIZE = 0xa0;
	const int RODATA_ADDRESS = 0xb4;

	//number of sections in this file, don't actually think I need it.
	//so i've commented it out.
	//xff.seekg(NUMBER_OF_SECTIONS, std::ios::beg);
	//xff.read(reinterpret_cast<char*>(&sectionCount), sizeof(int));
	xff.seekg(OFFSET, std::ios::beg); 
	xff.read(reinterpret_cast<char*>(&offset), sizeof(int));
	xff.seekg(RODATA_SIZE, std::ios::beg);
	xff.read(reinterpret_cast<char*>(&rodataSize), sizeof(int));
	xff.seekg(RODATA_ADDRESS, std::ios::beg);	//address of rodata segment
	xff.read(reinterpret_cast<char*>(&rodataAddress), sizeof(int));
}



void Xff::readHeaders(){
	xff.seekg(rodataAddress + offset,  std::ios::beg);
	readAndCheckMagic4("NMO\x00");

	xff.ignore(44); //ignoring these bytes, i haven't figured them out yet.
	xff.read(reinterpret_cast<char*>(&headers), sizeof(headers));

	//after reading the Section headers, goto the sections and read them
	xff.seekg(rodataAddress + headers.textures.address, std::ios::beg);
	textureHeaders.resize(headers.textures.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&textureHeaders[0])
				, headers.textures.numberOfEntries * sizeof(TextureHeader));

	//assert that the surface header is after the texture header;
	//headers should be contigious if they aren't something has gone wrong
	//the assert checks that the texture header is followed by the surface header
	assert(xff.tellg() == rodataAddress + headers.surfaces.address);
	surfaceHeaders.resize(headers.surfaces.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&surfaceHeaders[0])
				, headers.surfaces.numberOfEntries * sizeof(SurfaceHeader));

	//same with the geometry header
	assert(xff.tellg() == rodataAddress + headers.geometry.address);
	geometryHeaders.resize(headers.geometry.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&geometryHeaders[0])
				, headers.geometry.numberOfEntries * sizeof(GeometryHeader));
}



void Xff::parseVertices(){
	xff.seekg(rodataAddress, std::ios::beg);
	foreach(GeometryHeader geometry, geometryHeaders){
		assert((uint32_t)xff.tellg() - rodataAddress == geometry.offset);
		std::cout << "Geom " << geometry.entryCount << '\n';
		int numberOfAttributes = parseGeometryDataHeader();
		for(uint32_t i = 0; i < geometry.stripCount; i++){
			std::cout << "strip " << i << " of " << geometry.stripCount << '\n';
			int attributes = numberOfAttributes;
			parseStripHeader();
			parsePosition();
			Entry shortCut;
			xff >> shortCut;
			if(shortCut == Entry(1, 0, 0, 5)){
				//normals aren't always there either!
				xff.seekg(-4, std::ios_base::cur);
				attributes--;
				parseColour(attributes);
			} else{
				parseNormal(shortCut);
				attributes -= 2;
				parseColour(attributes);
			}

			if(attributes == 1){
				parseVertexWeight();
			} else if(attributes == 2){
				parseVertexWeight();
				//dormin2wanda.nmo parseSpecial
			}
			parseDouche();
		}
		Entry slide;
		Entry nopSlide(0, 0, 0, 0);
		xff >> slide;
		while(slide == nopSlide){
			xff >> slide;
		}
		xff.seekg(-4, std::ios_base::cur);
		//std::cout << "geometry " << i << " " << geometry.stripCount << '\n';
		assert(((uint32_t)xff.tellg() - rodataAddress) == geometry.size + geometry.offset);
		//std::cout << std::hex << "tellg: " << (uint32_t)xff.tellg();
	}
	std::cout << "finished!" ;
}



int Xff::parseGeometryDataHeader(){
	//The very first byte indicates how many sections there are i.e attributeNumbers.index
	Entry attribute, two, three;
	xff >> attribute >> three >> two;
	std::cout << std::hex << xff.tellg();
	std::cout << attribute<< '\n' << three << '\n' << two << '\n';
	assert(attribute.type == 0x01);
	assert(three.type == 0x03);
	assert(two.type == 0x02);
	return attribute.index;
}



void Xff::parseStripHeader(){
	Entry entry;
	xff >> entry;
	if(entry.index != 0 || entry.type != 0x6c){
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "strip header");
	}
	StartState startState = readObject<StartState>(xff);
	std::cout << startState << '\n';
}



void Xff::parsePosition(){
	Entry entry;
	xff >> entry;
	if(entry.index != 1 || entry.type != Entry::FLOAT32){
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "position error ");
	}

	boost::scoped_array<Ogre::Vector3> data(new Ogre::Vector3[entry.count]);
	xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(Ogre::Vector3));
	for(int i = 0; i < entry.count; i++)
		std::cout << data[i] << '\n';

}



void Xff::parseNormal(const Entry &entry){

//normals are optional
//	Entry entry;
//	xff >> entry;
	if(entry.type == Entry::FLOAT32){
		boost::scoped_array<Ogre::Vector3> data(new Ogre::Vector3[entry.count]);
		xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(Ogre::Vector3));
		for(int i = 0; i < entry.count; i++)
			std::cout << "normal32" << data[i] << '\n';
		return;
	} else if(entry.type == Entry::FLOAT16){
		for(int i = 0; i < entry.count; i++){
			Fixed16 fixed;
			xff.read(reinterpret_cast<char*>(&fixed), sizeof(fixed));
			std::cout << "normal16 " << fixed.getVector() << '\n';
		}
		return;
	}
	throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "normal error ");
}



void Xff::parseColour(int &attributesRemaining){
	Entry entry;
	xff >> entry;
	if(entry.type == 0x05){
		parseTexture();
		xff >> entry;
		attributesRemaining--;
	} 
	if(entry.type == Entry::COLOUR){
		boost::scoped_array<Colour> data(new Colour[entry.count]);
		xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(Colour));
		attributesRemaining--;
		for(int i = 0; i < entry.count; i++)
			std::cout << data[i] << '\n';
		return;
	}
	throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "colour error ");
}



void Xff::parseTexture(){
	Entry entry;
	xff >> entry;
	if(entry.type == Entry::UVMAP){
		boost::scoped_array<TextureMap> data(new TextureMap[entry.count]);
		xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(TextureMap));
		for(int i = 0; i < entry.count; i++)
			std::cout << data[i] << '\n';
		xff >> entry;
		assert(entry.type == 0x05);
		return;
	} else if(entry.type == Entry::FLOAT16){
		for(int i = 0; i < entry.count; i++){
			Fixed16 fixed;
			xff.read(reinterpret_cast<char*>(&fixed), sizeof(fixed));
			std::cout << "texture " << fixed.getVector() << '\n';
		}
		xff >> entry;
		assert(entry.type == 0x05);
		return;
	}
	throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "texture parse error ");
}



void Xff::parseVertexWeight(){
	Entry entry;
	xff >> entry;
	if(entry.type == Entry::VERTEXWEIGHT){
		boost::scoped_array<VertexWeight> entries(new VertexWeight[entry.count]);
		xff.read(reinterpret_cast<char*>(entries.get()), entry.count * sizeof(VertexWeight));
		for(int i = 0; i < entry.count; i++)
			std::cout << entries[i] << '\n';
		return;
	}
	throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "vertex weight parse error ");
}



void Xff::parseDouche(){
	Entry douche(0, 0, 0, 0x17);
	Entry slide; //fucking douche
	xff >> slide;
	Entry doucheCheck;
	xff >> doucheCheck;
	if(doucheCheck == douche){
		parseStripHeader();
		xff >> slide;
	} else {
		xff.seekg(-4, std::ios_base::cur);
	}
}
