#ifndef Skeleton_h
#define Skeleton_h
#include "Xff.hpp"
#include <stdint.h>
#include <string>
#include <vector>
#include <OgreVector4.h>
#include <OgreQuaternion.h>
/*
 * 5c - 6c addresses?
 * 0xc4 = number of tuple things bonecount + 2
 */
namespace sotc{
	struct Header {
		//uint32_t a; magic?
		uint32_t boneCount;
		uint32_t offset;
		uint32_t addressOfNames;
		uint32_t unknown[4];
		uint32_t optionalBoneCount;
		uint32_t optionalBonesAddress;
		uint32_t optionalBoneNamesAddress;
	};

	struct BoneHeader{
		uint32_t address;
		uint32_t unknown;
	};

	struct RawBone{
		uint32_t a, b, c;
		float unknown;
		float translation[4];
		float rotation[4];
		int32_t childLeft, childRight, parent;
		float d;
	};

	class Bone{
	public:
		Bone(const BoneHeader &header);
		void setName(std::string name);
		const std::string& getName() const;
		const BoneHeader& getHeader() const;
		Bone& setAttributes(const RawBone &raw);
		inline const Ogre::Vector4& getPosition() const{ return translation; }
		inline const Ogre::Quaternion& getRotation() const{ return rotation; }
		int32_t getParent() const { return parent; }
	private:
		inline Bone& setTranslation(const Ogre::Vector4 &vector);
		inline Bone& setRotation(const Ogre::Quaternion &quaternion); 
		inline Bone& setRelations(const uint32_t child0, const uint32_t child1, const uint32_t p);
		BoneHeader header;
		RawBone rawBone;
		std::string name;
		Ogre::Vector4 translation;
		Ogre::Quaternion rotation;
		int32_t childLeft, childRight, parent;
	};


	class Skeleton : public Xff<Skeleton> {
	public:
		Skeleton(const std::string &filename);
		void readHeaders(); 
		void readNames();
		void readBones();
		const std::vector<Bone>& getBones() const;
	private:
		std::vector<Bone> bones;
		Header header;
		std::vector<int> test;
	};
}

#endif
