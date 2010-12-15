#include "Skeleton.h"
#include <fstream>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
/*
 * 5c - 6c addresses?
 * 0xc4 = number of tuple things bonecount + 2
 */
using namespace sotc;
Bone::Bone(const BoneHeader &header) : header(header) { }



void Bone::setName(std::string name){
	this->name = name;
}



const std::string& Bone::getName() const {
	return name;
}



const BoneHeader& Bone::getHeader() const {
   	return header;
}


Bone& Bone::setAttributes(const RawBone &raw){
	setTranslation(Ogre::Vector4(raw.translation));
	setRotation(Ogre::Quaternion(raw.rotation[0]
				, raw.rotation[1], raw.rotation[2], raw.rotation[3]));
	setRelations(raw.childLeft, raw.childRight, raw.parent);
	rawBone = raw;
	return *this;
}



Bone& Bone::setTranslation(const Ogre::Vector4 &vector) {
   	translation = vector; return *this;
}



Bone& Bone::setRotation(const Ogre::Quaternion &quaternion) {
   	rotation = quaternion;
   	return *this; 
}



Bone& Bone::setRelations(const uint32_t child0, const uint32_t child1, const uint32_t p){
	childLeft = child0;
	childRight = child1;
	parent = p;
	return *this;
}

Skeleton::Skeleton(const std::string &filename) : Xff<Skeleton>(filename) {
	readNames();
	readBones();
}

const std::vector<Bone>& Skeleton::getBones() const{
   	return bones;
}



void Skeleton::readHeaders(){
	std::cout << test.size();
	test.resize(2);
	test.push_back(1);
	/*xff->seekg(rodataAddress + offset, std::ios::beg);
	readAndCheckMagic4("\x01\x00\x00\x00");
	xff->read(reinterpret_cast<char*>(&header), sizeof(header));

	std::vector<BoneHeader> boneHeaders;
	boneHeaders.reserve(header.boneCount);// + 2);
	boneHeaders.resize(header.boneCount);// + 2);
	xff->read(reinterpret_cast<char*>(&boneHeaders[0])
			, (header.boneCount) * sizeof(BoneHeader));
	foreach(const BoneHeader &header, boneHeaders)
		bones.push_back(Bone(header));
	*/
}

void Skeleton::readNames(){
	xff->seekg(rodataAddress + header.addressOfNames);
	foreach(Bone &bone, bones){
		char name[64];
		xff->getline(&name[0], 63, '\0');
		bone.setName(name);
	}
}

void Skeleton::readBones(){
	foreach(Bone &bone, bones){
		xff->seekg(bone.getHeader().address + rodataAddress);
		RawBone rawBone;
		xff->read(reinterpret_cast<char*>(&rawBone), sizeof(RawBone));
		bone.setAttributes(rawBone);
	}
}

