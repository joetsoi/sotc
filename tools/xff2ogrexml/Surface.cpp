#include <map>
#include "Surface.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
using namespace sotc;
Surface::Surface(const SurfaceHeader &surfaceHeader){
}

void Surface::uniqueVertices(){

	int vertexCount = 0;
	foreach(std::vector<Vertex> &strip, strips){
		std::vector<int> indexedVertexStrip;
		foreach(Vertex &vertex, strip){
			UniqueMap::iterator lb = uniqueMap.lower_bound(vertex);
			if(lb != uniqueMap.end() && !(uniqueMap.key_comp()(vertex, lb->first))){

			} else {
				uniqueMap.insert(lb, std::pair<Vertex, int>(vertex, vertexCount));
				//uniqueMap[vertex] == vertexCount;
				vertexCount++;
			}
			indexedVertexStrip.push_back(uniqueMap[vertex]);
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
