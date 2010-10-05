#include "tinyxml.h"
#include "XmlOut.h"
#include <boost/foreach.hpp>
#include "Surface.h"
#define foreach BOOST_FOREACH
using namespace sotc;

XmlOut::XmlOut( Xff &xff){
	TiXmlDocument doc;
	TiXmlElement *mesh = new TiXmlElement("mesh");
	doc.LinkEndChild(mesh);

	TiXmlElement *submeshes = new TiXmlElement("submeshes");
	mesh->LinkEndChild(submeshes);
	
	typedef std::pair<int, Surface> SurfacePair;
	foreach(SurfacePair surface, xff.getSurfaces()){
		surface.second.createTriangleList();
		TiXmlElement *submesh = new TiXmlElement("submesh");
		submeshes->LinkEndChild(submesh);

		submesh->SetAttribute("material", "testmaterial");
		submesh->SetAttribute("usesharedvertices", "false");

		TiXmlElement *faces = new TiXmlElement("faces");
		submesh->LinkEndChild(faces);

		foreach(const Triple &triangle, surface.second.getTriangles()){
			TiXmlElement *face = new TiXmlElement("face");
			faces->LinkEndChild(face);
			face->SetAttribute("v1", triangle.get<0>());
			face->SetAttribute("v2", triangle.get<1>());
			face->SetAttribute("v3", triangle.get<2>());
		}	
		//surface.second.uniqueVertices();
		TiXmlElement *geometry = new TiXmlElement("geometry");
		submesh->LinkEndChild(geometry);
		geometry->SetAttribute("vertexcount", surface.second.getUniqueMap().size());

		TiXmlElement *vertexbuffer = new TiXmlElement("vertexbuffer");
		geometry->LinkEndChild(vertexbuffer);

		foreach(const UniqueMap::value_type &pair, surface.second.getUniqueMap()){
			TiXmlElement *vertex = new TiXmlElement("vertex");
			vertexbuffer->LinkEndChild(vertex);
			vertexbuffer->SetAttribute("positions", "true");	

			TiXmlElement *position = new TiXmlElement("position");
			vertex->LinkEndChild(position);

			position->SetDoubleAttribute("x", pair.first.getPosition().x);
			position->SetDoubleAttribute("y", pair.first.getPosition().y);
			position->SetDoubleAttribute("z", pair.first.getPosition().z);
		}



	}
	doc.SaveFile(xff.filename + ".xml");
}

int main(int argc, char *argv[]){
//	std::ifstream xff(argv[1], std::ios::binary);
//	std::vector<char> buffer;
//	xff >> std::noskipws;
//	typedef std::istream_iterator<char> istream_iterator;
//	std::copy(istream_iterator(xff), istream_iterator(), std::back_inserter(buffer));
if(argc > 1){
	for(int i = 1; i < argc; i++){
		std::ifstream file(argv[i]);
		Xff xff(argv[i]);
		XmlOut out(xff);
	}
}else{

	Xff xff("/home/foobat/dormin/reeng/knight_A.nmo");
	XmlOut out(xff);
}
return 0;
}
