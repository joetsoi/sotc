#include <map>
#include "Surface.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
using namespace sotc;


Surface::Surface(const std::string &name, const SurfaceHeader &header) : name(name){
	texture[1] = header.textures[1].textureId;
	triangleCount = header.triangleCount;
}



void Surface::constructUniqueVertices(){
	int vertexCount = 0;
	foreach(const std::vector<Vertex> &strip, strips){
		std::vector<int> vertexIndex;
		foreach(Vertex vertex, strip){
			
			//find if they key already exists, if it doesn't add it to the vertex map
			UniqueMap::iterator lb = vertexMap.lower_bound(vertex);
			if(lb != vertexMap.end() && !(vertexMap.key_comp()(vertex, lb->first))){
				vertexIndex.push_back(vertexMap[vertex]);
			} else {
				vertexMap.insert(lb, std::pair<Vertex, int>(vertex, vertexCount));
				vertexIndex.push_back(vertexCount);
				vertexCount++;
			}
			//the vertex in this current iteration is number vertexCount, add it to
			//the vertex index
		}
		indexedVertices.push_back(vertexIndex);
	}
}



void Surface::constructTriangleList(){
	constructUniqueVertices();
	foreach(std::vector<int> strip, indexedVertices){	
		typedef std::vector<int>::const_iterator IntIterator;
		//converting from a triangle strip to lists means correcting the winding of
		//the triangles. every other triangle is facingthe other direction. Also
		//degenerate triangles(triangles with zero area/2 vertices are == ) must be
		//remooved.
		//
		//http://en.wikipedia.org/wiki/Triangle_strip 
		//
		//splitting into two for loops so i don't have to check the parity (odd/even)
		//of each triangle.
		//
		//as we're using a triangle list/soup, order of vertices shouldn't matter!
		//loop over even vertices add to soup (include zero)
		for(IntIterator i = strip.begin(); i < strip.end() - 2; i += 2){
			assert(strip.end() >= i+2);
			//from taking a look at the strips themselves it looks like 
			// v1 == v3 seems to be the common case, so i've put it in the first
			// condition here
			if(*i == *(i+2)) //|| *i == *(i+1) || *(i+1) == *(i+2))
				continue;//degenerate triangle ignore it
			else
				faces.push_back(Triangle(*i, *(i+1), *(i+2)));
		}

		//loop over odd vertices add to soup.
		for(IntIterator i = strip.begin() + 1; i < strip.end() - 2; i += 2){
			assert(strip.end() >= i+2);
			if(*i == *(i+2))// || *i == *(i+1) || *(i+1) == *(i+2))
				continue;//degenerate triangle ignore it
			else
				//faces.push_back(Triangle(*(i), *(i+1), *(i+2))); //note reverse orderin
				faces.push_back(Triangle(*(i+2), *(i+1), *i)); //note reverse orderin
		}
	}
}

