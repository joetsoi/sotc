#include "Xff.h"
#include <exception>
#include <stdint.h>
#include <cassert>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <OgreVector3.h>

#include "Vertex.h"
namespace sotc {
	enum eState {
		START,
		GET_POSITION,
		GET_NORMAL,
		GET_TEXTURE,
		GET_COLOUR,
		GET_BONES,
		FINISH_STRIP,
		GET_SPECIAL
	};

	class State{
		public:
			State(eState state, const Entry &entry) : state(state), entry(entry) {}
			eState state;
			Entry entry;
	};
}

using namespace sotc;


// my short hand for reading an object because i hate writing reinerpret cast
// replace with stream operator>> as this is just to make the code less clutered
template <typename T>
T readObject(std::ifstream &xff){
	T t;
	xff.read(reinterpret_cast<char*>(&t), sizeof(T));
	return t;
}


class ErrorOpeningFile : public std::runtime_error{
public:
	ErrorOpeningFile(const char *msg = "error opening file") : std::runtime_error(msg){}
};



class ParseException : public std::runtime_error {
public:
	ParseException(const char *msg = "") : std::runtime_error(msg){}

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



class MismatchedStrip : public ParseException {
public:
	MismatchedStrip() : ParseException(){}
};



Xff::Xff(const std::string &filename) : filename(filename){
	xff.open(filename.c_str(), std::ios::in|std::ios::binary);
	if(xff.is_open()){
		try{
			readAndCheckMagic4("xff\x00");
		} catch (BadMagicException &e){
			std::cerr << e.what() << " expected: " << e.expected
				<< " got: " << e.actual << std::endl;
		}
		readLocations();
		readHeaders();

		xff.close();
	} else {
		throw ErrorOpeningFile();
	}
}



std::string Xff::basename(){
	std::string base = filename.substr(filename.rfind("/") + 1);
	return base.replace(base.rfind(".nmo"), 4, "");
}



std::string Xff::withoutExtension(){
	return filename.substr(0, filename.rfind("."));
}



void Xff::readAndCheckMagic4(const std::string &expected){
	char magic[5];
	xff.read(magic, 4);
	magic[4] = '\0';
	//perhaps substring expected down to 3 characters?
	//doesn't account for length expected > length magic eg. if expected=xffMAGIC
	//it would throw the exception, i figured it shouldn't make a huge difference
	if(expected.compare(std::string(magic)) != 0){
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

	SectionHeaders headers;
	xff.read(reinterpret_cast<char*>(&headers), sizeof(headers));

	//after reading the Section headers, goto the sections and read them
	std::vector<TextureHeader> textureHeaders;
	xff.seekg(rodataAddress + headers.textures.address, std::ios::beg);
	textureHeaders.resize(headers.textures.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&textureHeaders[0])
			, headers.textures.numberOfEntries * sizeof(TextureHeader));

	//assert that the surface header is after the texture header;
	//headers should be contigious if they aren't something has gone wrong
	//the assert checks that the texture header is followed by the surface header
	assert(xff.tellg() == rodataAddress + headers.surfaces.address);
	std::vector<SurfaceHeader> surfaceHeaders;
	surfaceHeaders.resize(headers.surfaces.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&surfaceHeaders[0])
			, headers.surfaces.numberOfEntries * sizeof(SurfaceHeader));

	//same with the geometry header
	assert(xff.tellg() == rodataAddress + headers.geometry.address);
	std::vector<GeometryHeader> geometryHeaders;
	geometryHeaders.resize(headers.geometry.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&geometryHeaders[0])
			, headers.geometry.numberOfEntries * sizeof(GeometryHeader));

	readNames(textureHeaders, surfaceHeaders);
	stateParse(geometryHeaders);
}



void Xff::readNames(std::vector<TextureHeader> textureHeaders
					, std::vector<SurfaceHeader> surfaceHeaders){
	xff.seekg(rodataAddress + textureHeaders[0].addressOfName);
	foreach(TextureHeader texture, textureHeaders){
		//xff.seekg(rodataAddress + texture.addressOfName, std::ios::beg);
		char name[64];
		xff.getline(&name[0], 63, '\0');
		textures.push_back(std::string(name));
		std::cout << name << '\n';
	}
	assert(xff.tellg() == rodataAddress + surfaceHeaders[0].addressOfNames);
	int surfaceNumber = 0;
	foreach(SurfaceHeader surface, surfaceHeaders){
		//xff.seekg(rodataAddress + surface.addressOfNames, std::ios::beg);
		//assuming no name is over 63 characters long!
		char name[64];
		//but it's ok, only a maximum of 63 are read.
		xff.getline(&name[0], 63, '\0');
		
		surfaces.insert(std::pair<uint32_t, Surface>(surfaceNumber, Surface(name, surface)));


		//set surface vertex/triangle/stripcount;
		++surfaceNumber;
	}
}



void Xff::stateParse(std::vector<GeometryHeader> geometryHeaders){
	std::vector<boost::function<State (const GeometryHeader &header
						, const Entry &entry, std::vector<Vertex> &vertices)> > stateTable;

	stateTable.push_back(boost::bind(&Xff::runStart, this, _1, _2, _3));
	stateTable.push_back(boost::bind(&Xff::runGetPosition, this, _1, _2, _3));
	stateTable.push_back(boost::bind(&Xff::runGetNormal, this, _1, _2, _3));
	stateTable.push_back(boost::bind(&Xff::runGetTexture, this, _1, _2, _3));
	stateTable.push_back(boost::bind(&Xff::runGetColour, this, _1, _2, _3));
	stateTable.push_back(boost::bind(&Xff::runGetBones, this, _1, _2, _3));
	stateTable.push_back(boost::bind(&Xff::runFinishStrip, this, _1, _2, _3));

	xff.seekg(rodataAddress, std::ios::beg);

	// this is a stupid variable i hate it, it's all because of me not figuring out when
	// there is an extra section of (0x17,start,0x17) or a bunch of 0s at the end of each
	// strip of geometry section, meaning i have to align the while loop below with +4
	// as reading this means i'm off alignment with geometry.size by + 4. FSFfdsffarafda
	xff >> last;
	foreach(GeometryHeader geometry, geometryHeaders){
		//if(geometry.surface >= surfaces.size())
		//	surfaces.push_back(Surface());
		//std::cout << "geom : " << geometry.surface << " surf: " << surfaces.size() << '\n';

		//int numberOfAttributes = parseGeometryDataHeader();
		parseGeometryDataHeader(last);
		State currentState = State(START, last);

		//vertices is changed by each of the states and in
		//runFinish strip is added to the surface, this doesn't feel as neat
		//as it should be
		std::vector<Vertex> vertices;
		while((uint32_t)xff.tellg() - rodataAddress != geometry.offset + geometry.size + 4){
			try {
				//function invocation
				currentState = stateTable.at(currentState.state)
					(geometry, currentState.entry, vertices);
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



State Xff::runStart(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices){
	if(entry.index != 0 || entry.type != 0x6c){
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "strip header");
	}
		
	StartState startState = readObject<StartState>(xff);
	Entry nextEntry;
	xff >> nextEntry;
	return State(GET_POSITION, nextEntry);
}



State Xff::runGetPosition(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices){
	if(entry.index != 1 || entry.type != Entry::FLOAT32){
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "position error ");
	}

	std::vector<Ogre::Vector3> data;
	data.resize(entry.count);
	xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(Ogre::Vector3));

	foreach(Ogre::Vector3 vector, data)
		vertices.push_back(Vertex(vector));

	Entry nextEntry;
	xff >> nextEntry;
	if(Entry(1, 0, 0, 5) == nextEntry){
		return State(GET_TEXTURE, nextEntry);
	}
	return State(GET_NORMAL, nextEntry);
}



State Xff::runGetNormal(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices){
	if(entry.index != 2){
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "normal error ");
	}

	if(entry.type == Entry::FLOAT32){
		std::vector<Ogre::Vector3> normals(entry.count);
		xff.read(reinterpret_cast<char*>(&normals[0]), entry.count * sizeof(Ogre::Vector3));

		if(vertices.size() != normals.size())
			throw MismatchedStrip();

		std::vector<Ogre::Vector3>::iterator it = normals.begin();
		foreach(Vertex &vertex, vertices){
			vertex.setNormal(*it);
			++it;
		}

	} else if(entry.type == Entry::FLOAT16){
		std::vector<Fixed16> normals(entry.count);
		xff.read(reinterpret_cast<char*>(&normals[0]), entry.count * sizeof(Fixed16));
		if(vertices.size() != normals.size())
			throw MismatchedStrip();

		std::vector<Fixed16>::iterator it = normals.begin();
		foreach(Vertex &vertex, vertices){
			vertex.setNormal(it->getVector());
			++it;
		}
	} else {
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), entry, "normal error ");
	}

	Entry nextEntry;
	xff >> nextEntry;
	if(Entry(1, 0, 0, 5) == nextEntry)
		return State(GET_TEXTURE, nextEntry);
	
	return State(GET_COLOUR, nextEntry);
}



State Xff::runGetTexture(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices){
	Entry nextEntry;
	xff >> nextEntry;
	if(nextEntry.type == Entry::UVMAP){
		std::vector<TextureMap> data(nextEntry.count);
		xff.read(reinterpret_cast<char*>(&data[0]), nextEntry.count * sizeof(TextureMap));

		if(vertices.size() != data.size())
			throw MismatchedStrip();

		std::vector<TextureMap>::iterator it = data.begin();
		foreach(Vertex &vertex, vertices){
			vertex.setUvMap(*it);
			++it;
		}
	} else if(nextEntry.type == Entry::FLOAT16){
		std::vector<Fixed16> textures(nextEntry.count);
		xff.read(reinterpret_cast<char*>(&textures[0]), nextEntry.count * sizeof(Fixed16));

		//FIXME add UVW to Vertex class


	} else {
		throw MalformedEntry(static_cast<uint32_t>(xff.tellg()), nextEntry, "texture parse error ");
	}

	xff >> nextEntry;
	assert(nextEntry.type == 0x05);
	xff >> nextEntry;
	return State(GET_COLOUR, nextEntry);
}



State Xff::runGetColour(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices){
	if(entry.type == Entry::COLOUR){
		std::vector<Colour> data(entry.count);
		xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(Colour));
		
		if(vertices.size() != data.size())
			throw MismatchedStrip();

		std::vector<Colour>::iterator it = data.begin();
		foreach(Vertex &vertex, vertices){
			vertex.setColour(*it);
			++it;
		}
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



State Xff::runGetBones(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices){
	if(entry.type == Entry::VERTEXWEIGHT){
		std::vector<VertexWeight> entries(entry.count);
		xff.read(reinterpret_cast<char*>(&entries[0]), entry.count * sizeof(VertexWeight));

		if(vertices.size() != entries.size())
			throw MismatchedStrip();

		std::vector<VertexWeight>::iterator it = entries.begin();
		foreach(Vertex &vertex, vertices){
			vertex.setBoneWeight(*it);
			++it;
		}
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


State Xff::runFinishStrip(const GeometryHeader &head, const Entry &entry, std::vector<Vertex> &vertices){

	if(vertices.size() > 0){
		surfaces[head.surface].addStrip(vertices);
		vertices.clear();
	}


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
