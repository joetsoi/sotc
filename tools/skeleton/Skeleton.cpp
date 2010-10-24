#include "Xff.hpp"
#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include <boost/iostreams/device/mapped_file.hpp>
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

	class Bone{
	public:
		Bone(const BoneHeader &header) : header(header) { }
		inline void setName(std::string name){
			this->name = name;
		}
		inline const BoneHeader& getHeader() const { return header; }
	private:
		BoneHeader header;
		std::string name;
	};

	class Skeleton : public Xff<Skeleton> {
	public:
		Skeleton(const std::string &filename) : Xff<Skeleton>(filename) {
			readHeaders();
			readNames();
			readBones();
		}
		void readHeaders(); 
		void readNames();
		void readBones();
	private:
		std::vector<Bone> bones;
		Header header;
	};

	void Skeleton::readHeaders(){
		xff->seekg(rodataAddress + offset, std::ios::beg);
		readAndCheckMagic4("\x01\x00\x00\x00");
		xff->read(reinterpret_cast<char*>(&header), sizeof(header));

		std::vector<BoneHeader> boneHeaders;
		boneHeaders.reserve(header.boneCount + 2);
		boneHeaders.resize(header.boneCount + 2);
		xff->read(reinterpret_cast<char*>(&boneHeaders[0])
				, (header.boneCount + 2) * sizeof(BoneHeader));
		foreach(const BoneHeader &header, boneHeaders)
			bones.push_back(Bone(header));
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
		struct RawBone{
			uint32_t childLeft, childRight, parent;
			float bones[9];
			int32_t a, b, c;
			float d;
		};
		foreach(Bone &bone, bones){
			xff->seekg(bone.getHeader().address + rodataAddress);
			RawBone rawBone;
			xff->read(reinterpret_cast<char*>(&rawBone), sizeof(RawBone));
			std::cout << rawBone.a << '\n';
		}
	}
}

int main(){
	sotc::Skeleton skeleton("/home/foobat/dormin/reeng/knight_A.skb");
	boost::iostreams::mapped_file test("/home/foobat/dormin/reeng/knight_A.skb");
	std::cout << test.size() << '\n';
	const int* offset = reinterpret_cast<const int*>(test.const_data() + 0x4c);
	const int* rodataSize = reinterpret_cast<const int*>(test.const_data() + 0xa0);
	const int* rodata = reinterpret_cast<const int*>(test.const_data() + 0xb4);
	std::cout << *rodata << '\n';
	return 0;
}
