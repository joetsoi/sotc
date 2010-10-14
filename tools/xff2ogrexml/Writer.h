#ifndef Writer_h
#define Writer_h
#include <string>
#include "Model.h"
namespace sotc {
	class Writer{
		public:
			Writer(Model &model, bool hasDestination = false, std::string outputDirectory = "");
			void generateXml(Model &model);
			void generateMaterials(Model &model);
		private:
			bool hasDestination;
			std::string outputDirectory;
	};
}
#endif
