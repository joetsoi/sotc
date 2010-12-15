#include <stdint.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "Xff.hpp"
namespace sotc{
	struct Header{
		uint32_t a[5];
		uint32_t numberOfBones;
		uint32_t b[11];
	};

	class Animation : public Xff<Animation> {
	public:
		Animation(const std::string &filename) : Xff<Animation>(filename){
		}
		void readHeaders(){
			xff->seekg(rodataAddress + offset, std::ios::beg);
			xff->read(reinterpret_cast<char*>(&header), sizeof(Header));
			std::cout << header.numberOfBones << '\n';
			std::cout << header.a[4] << '\n' << header.a[2] << '\n';
			std::vector<int> boneD(header.numberOfBones);
			xff->read(reinterpret_cast<char*>(&boneD[0]), sizeof(boneD));

			std::vector<int> test;
			xff->seekg(rodataAddress + offset + header.a[4]);
			test.resize(header.numberOfBones);
			xff->read(reinterpret_cast<char*>(&test[0])
					, sizeof(int) * header.numberOfBones);
			std::cout << test.at(0); 



		}		
	private:
		Header header;
	};

}

int main(int argc, char* argv[]){
	sotc::Animation animation("/home/jtsoi/dormin/reeng/minotaur_A_standby.anb");
	return 0;
}
