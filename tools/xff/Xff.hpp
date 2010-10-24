#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
namespace sotc{

class ErrorOpeningFile : public std::runtime_error{
public:
	ErrorOpeningFile(const char *msg = "error opening file") : std::runtime_error(msg){}
};



class ParseException : public std::runtime_error {
public:
	ParseException(const char *msg = "") : std::runtime_error(msg){}

};



class BadMagicException : public ParseException {
public:
	BadMagicException(const char *expected, const char *actual)
		: ParseException("Bad Magic Exception")
		, expected(expected), actual(actual){}
	const char *expected;
	const char *actual;
};



template<typename T>
class Xff {
public:
	Xff(const std::string &filename);
	void readAndCheckMagic4(const std::string &expected);
	void readLocations();
	//inline void readHeaders(){
	//	static_cast<T*>(this)->readHeaders();
	//}
protected:
	int offset;
	int rodataAddress;
	int rodataSize;
	std::string filename;
	boost::shared_ptr<std::ifstream> xff;
};



template<typename T>
Xff<T>::Xff(const std::string &filename) : filename(filename)
   , xff(new std::ifstream(filename.c_str(), std::ios::in|std::ios::binary)) {
	if(xff->is_open()){
		try{
			readAndCheckMagic4("xff\x00");
			readLocations();
		} catch (BadMagicException &e){
			std::cerr << e.what() << " expected: " << e.expected
				<< " got: " << e.actual << std::endl;
		}
	} else {
		throw ErrorOpeningFile();
	}	
}	



template<typename T>
void Xff<T>::readAndCheckMagic4(const std::string &expected){
	char magic[5];
	xff->read(magic, 4);
	magic[4] = '\0';
	if(expected.compare(std::string(magic)) != 0){
		throw BadMagicException(expected.c_str(), magic);
	}
}



template<typename T>
void Xff<T>::readLocations(){
	//0x0 - 0xB = 78 66 66 00  00 00 00 00  00 00 00 00
	//0xC uint32_t : skb = 0, nmo = 1, nto = 1
	//0x14 int = xff file size
	//const int NUMBER_OF_SECTIONS = 0x40;
	const int OFFSET = 0x4c;//distance of section headers from beginning of file;
	const int RODATA_SIZE = 0xa0;
	const int RODATA_ADDRESS = 0xb4;

	//number of sections in this file, don't actually think I need it.
	//so i've commented it out.
	//xff->seekg(NUMBER_OF_SECTIONS, std::ios::beg);
	//xff->read(reinterpret_cast<char*>(&sectionCount), sizeof(int));
	xff->seekg(OFFSET, std::ios::beg); 
	xff->read(reinterpret_cast<char*>(&offset), sizeof(int));
	xff->seekg(RODATA_SIZE, std::ios::beg);
	xff->read(reinterpret_cast<char*>(&rodataSize), sizeof(int));
	xff->seekg(RODATA_ADDRESS, std::ios::beg);	//address of rodata segment
	xff->read(reinterpret_cast<char*>(&rodataAddress), sizeof(int));
}

}
