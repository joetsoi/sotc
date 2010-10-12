#ifndef Writer_h
#define Writer_h
#include <string>
#include "Xff.h"
namespace sotc {
	class Writer{
		public:
			Writer(Xff &xff, bool hasDestination = false, std::string outputDirectory = "");
			void generateXml(Xff &xff);
			void generateMaterials(Xff &xff);
		private:
			bool hasDestination;
			std::string outputDirectory;
	};
}
#endif
