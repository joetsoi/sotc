#include <map>
#include "Surface.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
using namespace sotc;
Surface::Surface(const SurfaceHeader &surfaceHeader){
}

void Surface::uniqueVertices(){

	foreach(std::vector<Vertex> &strip, strips){
		std::vector<int> indexedVertexStrip;
		foreach(Vertex &vertex, strip){
			uniqueMap.insert(std::pair<Vertex, int>(vertex, vertexCount));
			indexedVertexStrip.push_back(uniqueMap[vertex]);
			vertexCount++;
		}
		indexedVertices.push_back(indexedVertexStrip);
	}
}

void Surface::createTriangleList(){
	if(faces.size() == 0){
		if(uniqueMap.size() == 0)
			uniqueVertices();
		foreach(std::vector<int> indexStrip, indexedVertices){
			for(std::vector<int>::const_iterator i = indexStrip.begin(); i < indexStrip.end() - 2; i++){
				if(*i == *(i+2) || *i == *(i+1) || *(i+1) == *(i+2))
					continue;
				else {
					faces.push_back(Triple(*i, *(i+1), *(i+2)));
				}

			}
		}
	}
}
