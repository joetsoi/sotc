#include <exception>
#include <stdint.h>
#include <string.h>
#include <cassert>
#include "Xff.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include <boost/function.hpp>
#include <boost/bind.hpp>

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
			stateParse();
		} catch (BadMagicException &e){
			std::cerr << e.what() << " expected: " << e.expected
				<< " got: " << e.actual << std::endl;
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



void Xff::stateParse(){
	std::vector<boost::function<State (Entry)> > stateTable;

	stateTable.push_back(boost::bind(&Xff::runStart, this, _1));
	stateTable.push_back(boost::bind(&Xff::runGetPosition, this, _1));
	stateTable.push_back(boost::bind(&Xff::runGetNormal, this, _1));
	stateTable.push_back(boost::bind(&Xff::runGetTexture, this, _1));
	stateTable.push_back(boost::bind(&Xff::runGetColour, this, _1));
	stateTable.push_back(boost::bind(&Xff::runGetBones, this, _1));
	stateTable.push_back(boost::bind(&Xff::runFinishStrip, this, _1));

	xff.seekg(rodataAddress, std::ios::beg);
	// this is a stupid variable i hate it, it's all because of me not figuring out when
	// there is an extra section of (0x17,start,0x17) or a bunch of 0s at the end of each
	// strip of geometry section, meaning i have to align the while loop below with +4
	// as reading this means i'm off alignment with geometry.size by + 4. FSFfdsffarafda
	xff >> last;
	foreach(GeometryHeader geometry, geometryHeaders){
		//int numberOfAttributes = parseGeometryDataHeader();
		parseGeometryDataHeader(last);
		State currentState = State(START, last);
		while((uint32_t)xff.tellg() - rodataAddress != geometry.offset + geometry.size + 4){
			try {
				currentState = stateTable.at(currentState.state)(currentState.entry);
			} catch (std::out_of_range) {
				std::cerr << "No corresponding function in stateTable for enum eState[" 
						<< currentState.state << ']' <<  std::endl;
				std::terminate();
			} catch(MalformedEntry &e){
				std::cerr << "Malformed Entry error, terminated: ";
				std::cerr << std::hex << e.what() << " " << e.entry << " "
					 << "0x" << e.location << std::endl;
				std::terminate();
			}
		}
		assert(((uint32_t)xff.tellg() - rodataAddress) == geometry.size + geometry.offset + 4);
	}
}



Xff::State Xff::runStart(const Entry &entry){
	if(entry.index != 0 || entry.type != 0x6c){
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "strip header");
	}
		
	StartState startState = readObject<StartState>(xff);
	Entry nextEntry;
	xff >> nextEntry;
	return State(GET_POSITION, nextEntry);
}



Xff::State Xff::runGetPosition(const Entry &entry){
	if(entry.index != 1 || entry.type != Entry::FLOAT32){
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "position error ");
	}

	std::vector<Ogre::Vector3> data;
	data.resize(entry.count);
	xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(Ogre::Vector3));
	Entry nextEntry;
	xff >> nextEntry;
	if(Entry(1, 0, 0, 5) == nextEntry){
		return State(GET_TEXTURE, nextEntry);
	}
	return State(GET_NORMAL, nextEntry);
}



Xff::State Xff::runGetNormal(const Entry &entry){
	if(entry.index != 2){
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "normal error ");
	}

	if(entry.type == Entry::FLOAT32){
		std::vector<Ogre::Vector3> normals(entry.count);
		xff.read(reinterpret_cast<char*>(&normals[0]), entry.count * sizeof(Ogre::Vector3));
	} else if(entry.type == Entry::FLOAT16){
		std::vector<Fixed16> normals(entry.count);
		xff.read(reinterpret_cast<char*>(&normals[0]), entry.count * sizeof(Fixed16));
	} else {
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "normal error ");
	}

	Entry nextEntry;
	xff >> nextEntry;
	if(Entry(1, 0, 0, 5) == nextEntry)
		return State(GET_TEXTURE, nextEntry);
	
	return State(GET_COLOUR, nextEntry);
}



Xff::State Xff::runGetTexture(const Entry &entry){
	Entry nextEntry;
	xff >> nextEntry;
	if(nextEntry.type == Entry::UVMAP){
		std::vector<TextureMap> data(nextEntry.count);
		xff.read(reinterpret_cast<char*>(&data[0]), nextEntry.count * sizeof(TextureMap));
	} else if(nextEntry.type == Entry::FLOAT16){
		std::vector<Fixed16> textures(nextEntry.count);
		xff.read(reinterpret_cast<char*>(&textures[0]), nextEntry.count * sizeof(Fixed16));
	} else {
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), nextEntry, "texture parse error ");
	}

	xff >> nextEntry;
	assert(nextEntry.type == 0x05);
	xff >> nextEntry;
	return State(GET_COLOUR, nextEntry);
}



Xff::State Xff::runGetColour(const Entry &entry){
	if(entry.type == Entry::COLOUR){
		std::vector<Colour> data(entry.count);
		xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(Colour));
	} else {
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "colour error ");
	}
	Entry nextEntry;
	xff >> nextEntry;
	if(nextEntry == Entry(0, 0, 0, 0x17)){
		return State(FINISH_STRIP, nextEntry);
	}
	return State(GET_BONES, nextEntry);
}



Xff::State Xff::runGetBones(const Entry &entry){
	if(entry.type == Entry::VERTEXWEIGHT){
		std::vector<VertexWeight> entries(entry.count);
		xff.read(reinterpret_cast<char*>(&entries[0]), entry.count * sizeof(VertexWeight));
	} else {
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "vertex weight parse error ");
	}
	Entry nextEntry;
	xff >> nextEntry;
	if(nextEntry == Entry(0, 0, 0, 0x17)){
		return State(FINISH_STRIP, nextEntry);
	} // else GET SPECIAL
	throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "end of strip != 0x17");
}


Xff::State Xff::runFinishStrip(const Entry& entry){
	Entry douche(0, 0, 0, 0x17);
	xff >> last;
	if(last == douche){
		xff >> last;
		if(last.index != 0 || last.type != 0x6c){
			throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), last, "douche header");
		}
		StartState startState = readObject<StartState>(xff);
		xff >> last; //0x17 again
		xff >> last;
	}
	
	if(last == Entry(0, 0, 0, 0)){
		//no operation slide if entry is 0, 0, 0, 0;
		xff >> last;
		while(last == Entry(0, 0, 0, 0)){
			xff >> last;
		}
	}
	return State(START, last);
}



int Xff::parseGeometryDataHeader(const Entry &entry){
	//The very first byte indicates how many sections there are i.e attributeNumbers.index
	Entry two, three;
	xff >> three >> two;
	assert(entry.type == 0x01);
	assert(three.type == 0x03);
	assert(two.type == 0x02);
	xff >> last;
	return entry.index;
}
