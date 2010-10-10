#ifndef Writer_h
#define Writer_h
#include "Xff.h"
namespace sotc {
	class Writer{
		public:
			Writer(Xff &xff);
			void generateXml(Xff &xff);
			void generateMaterials(Xff &xff);
	};
}
#endif
