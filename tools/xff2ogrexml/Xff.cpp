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

Xff::Xff(const char *filename){
	std::ifstream xff(filename, std::ios::binary);
	readLocations(xff);
//	std::cout << "Reading Headers" << '\n';
	readHeaders(xff);
//	std::cout << "Reading vertices" << '\n';
	readVertices(xff);
	xff.close();

//	foreach(Surface &surface, getSurfaces()){
	//	surface.uniqueVertices();
//		surface.createTriangleList();
//	}
/*
	foreach(Surface surface, getSurfaces()){
		foreach(const UniqueMap::value_type &pair, surface.getUniqueMap()){
			std::cout << pair.second << " " << pair.first.getPosition() << '\n';
		
		}
	}
*/
}

void Xff::readLocations(std::ifstream &xff){
	//xff.seekg(16 * sizeof(int), std::ios::beg);
	//xff.read(reinterpret_cast<char*>(&sectionCount), sizeof(int));
	xff.seekg(0x4c, std::ios::beg); //distance of NMO from beginning of nmo
	xff.read(reinterpret_cast<char*>(&offset), sizeof(int));
	xff.seekg(0xa0, std::ios::beg);
	xff.read(reinterpret_cast<char*>(&rodataSize), sizeof(int));
	xff.seekg(0xb4, std::ios::beg);	//address of rodata segment
	xff.read(reinterpret_cast<char*>(&rodataAddress), sizeof(int));
}

void Xff::readHeaders(std::ifstream &xff){
	xff.seekg(rodataAddress + offset, std::ios::beg);
	char magic[4];
	xff.read(magic, sizeof(magic));
	int magicTest = strcmp("NMO\x00", magic);
	assert(magicTest == 0);
	//whenever i ignore bytes it's because i don't know what they do
	xff.ignore(44);
	xff.read(reinterpret_cast<char*>(&headers), sizeof(headers));

	//after reading the Section headers, goto the sections and read them
	xff.seekg(rodataAddress + headers.textures.address, std::ios::beg);
	textureHeaders.resize(headers.textures.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&textureHeaders[0]), headers.textures.numberOfEntries * sizeof(TextureHeader));

	//assert that the surface header is after the texture header;
	assert(xff.tellg() == rodataAddress + headers.surfaces.address);
	surfaceHeaders.resize(headers.surfaces.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&surfaceHeaders[0]), headers.surfaces.numberOfEntries * sizeof(SurfaceHeader));

	//same with the geometry header
	assert(xff.tellg() == rodataAddress + headers.geometry.address);
	geometryHeaders.resize(headers.geometry.numberOfEntries);
	xff.read(reinterpret_cast<char*>(&geometryHeaders[0]), headers.geometry.numberOfEntries * sizeof(GeometryHeader));
}

void Xff::readVertices(std::ifstream &xff){
	xff.seekg(rodataAddress, std::ios::beg);

	foreach(GeometryHeader geometry, geometryHeaders){
		if(surfaces.size() <= geometry.surface)
			surfaces.push_back(Surface());
		Entry one = readObject<Entry>(xff);
		Entry three = readObject<Entry>(xff);
		Entry two = readObject<Entry>(xff);
		
		//I'm not sure what these entries do, just assert that they will follow
		//the the pattern 1, 3, 2 and if they don't i can find out then
		assert(one.type == 0x01);
		assert(three.type == 0x03);
		assert(two.type == 0x02);

		int entrySum = 0;
		while(static_cast<uint32_t>(xff.tellg()) - rodataAddress - geometry.offset != geometry.size){
			int addy = xff.tellg();
			Entry entry = readObject<Entry>(xff);
			switch(entry.type){ //should i change the switch?
				case Entry::VERTEX:
				{
					assert(entry.index == 1 || entry.index == 2);
					boost::scoped_array<Ogre::Vector3> data(new Ogre::Vector3[entry.count]);
					//std::vector<Ogre::Vector3> data;
					//data.resize(entry.count);
					xff.read(reinterpret_cast<char*>(&data[0]), entry.count * sizeof(Ogre::Vector3));

					//vertices.reserve(vertices.size() + entry.count);
					if(1 == entry.index){ //entry is a vertex position
						std::vector<Vertex> strip;
						for(int i = 0; i < entry.count; i++){
							strip.push_back(Vertex(data[i], geometry.surface));
					//		std::cout << Vertex(data[i], geometry.surface) << '\n';
						}
						//surfaces[geometry.surface].addStrip(strip);
					} else if(entry.index == 2) {
						for(int i = 0; i < entry.count; i++){
						}
							//surfaces[geometry.surface].getVertexFromLastStrip(i).setNormal(data[i]);
							//surfaces[geometry.surface].strips.back().at(i).setNormal(data[i]);
							//ok this is ridiculous fix this. wtf does this even mean to someone who
							//has never seen this before.
							//before i forget set the normal of the vertex i in the current last
							//or working strip of the surface that this piece of geometry belnogs to
							//strip.at(i).setNormal(data[i]);
							//surfaces[geometry.surface].vertices.at(i).setNormal(data[i]);
					} else {
						std::cout << "VERTEX ! 1|2" << entry.index << " " << entry.count << '\n';
					}
					//std::cout << std::hex << xff.tellg() << '\n';
		//			std::cout << std::hex << 0x68 << '\n';
					break;
				}
				case Entry::VERTEXWEIGHT:
				{
					//read all entries
					if(0 != entry.index){
						assert(entry.index == 5);
						boost::scoped_array<VertexWeight> entries(new VertexWeight[entry.count]);
						xff.read(reinterpret_cast<char*>(entries.get()), entry.count * sizeof(VertexWeight));
						for(int i = 0; i < entry.count; i++){
						}
							//surfaces[geometry.surface].getVertexFromLastStrip(i).setBoneWeight(entries[i]);
							//surfaces[geometry.surface].strips.back().at(i).setBoneWeight(entries[i]);
							//strip.at(i).setBoneWeight(entries[i]);
							//surfaces[geometry.surface].vertices.at(i).setBoneWeight(entries[i]);
					} else {
						StartState startState = readObject<StartState>(xff);
					/*i have no idea what this type of entry is. */ 
					}
					//std::cout << std::hex << xff.tellg() << '\n';
		//			std::cout << std::hex << 0x6c << '\n';
					break;
				}
				case Entry::NORMALTANGENT16BIT: {
					assert(entry.index == 2 || entry.index == 3);
					for(int i = 0; i < entry.count; i++){
							short data[4] = {0, 0, 0, 0}; //not sure what type it is yet!
							xff.read(reinterpret_cast<char*>(&data), sizeof(data));
						if(2 == entry.index){
							Ogre::Vector3 normal(data[0] / 4096.0, data[1] / 4096.0,
												data[2] / 4096.0);
							//assert(data[4] != 0);
							//surfaces[geometry.surface].vertices.at(i).setNormal(normal);
							//surfaces[geometry.surface].getVertexFromLastStrip(i).setNormal(normal);
							//surfaces[geometry.surface].strips.back().at(i).setNormal(normal);
							//strip.at(i).setNormal(normal);
							//normal.a = data[0] / 4096.0;
						} else if(3 == entry.index){
							Ogre::Vector4 tangent(data[0] / 4096.0, data[1] / 4096.0,
												data[2] / 4096.0, data[3] / 4096.0);
							//vertices.at(i).setTangent(tangent);?
							//i'm not actually sure if this is the tangent
							//so i'm not doing anything with it for now!
							//this is pretending i will come back later and sort it out
						}
					}
					//std::cout << std::hex << xff.tellg() << '\n';
		//			std::cout << std::hex << 0x6d << '\n';
					break;
				}
				case Entry::COLOUR:
					assert(entry.index == 4);
					for(int i = 0; i < entry.count; i++)
						readObject<Colour>(xff);
						//strip.at(i).setColour(readObject<Colour>(xff));
						//surfaces[geometry.surface].getVertexFromLastStrip(i).setColour(readObject<Colour>(xff));
					//std::cout << std::hex << xff.tellg() << '\n';
					//std::cout << std::hex << 0x6e << '\n';
					break;
						//surfaces[geometry.surface].vertices.at(i).setColour(readObject<Colour>(xff));
				case Entry::UVMAP:
					assert(entry.index == 3);
					for(int i = 0; i < entry.count; i++)
						readObject<TextureMap>(xff);
						//surfaces[geometry.surface].getVertexFromLastStrip(i).setUvMap(readObject<TextureMap>(xff));
						//strip.at(i).setUvMap(readObject<TextureMap>(xff));
						//surfaces[geometry.surface].vertices.at(i).setUvMap(readObject<TextureMap>(xff));
					//std::cout << std::hex << xff.tellg() << '\n';
					//std::cout << std::hex << 0x65 << '\n';
					break;

				case 0x17:
					entrySum++;
					//std::cout << xff.tellg() << "0x17 end?" << std::endl;
					break;
				case 0x05: // double entry count
					//std::cout << (int)entry.index << " " << (int)entry.mask << " " << (int)entry.count << '\n';
					//std::cout << std::hex << xff.tellg() << '\n';
					//std::cout << std::hex << "0x05"  << '\n';
					//entrySum++;
					break;
			}
		}
		//std::cout << entrySum << " " << geometry.entryCount << " " << geometry.stripCount<< std::endl;
	}
}


