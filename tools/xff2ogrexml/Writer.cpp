#include "Writer.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include "tinyxml.h"

#include "Surface.h"
#include "Entry.h"
using namespace sotc;

typedef std::pair<int, Surface> SurfacePair;
Writer::Writer(Xff &xff){
	generateXml(xff);
	//generateMaterials(xff);
}

void Writer::generateXml(Xff &xff){
	TiXmlDocument doc;
	TiXmlElement *mesh = new TiXmlElement("mesh");
	doc.LinkEndChild(mesh);

	TiXmlElement *submeshes = new TiXmlElement("submeshes");
	mesh->LinkEndChild(submeshes);

	TiXmlElement *submeshnames = new TiXmlElement("submeshnames");
	mesh->LinkEndChild(submeshnames);
	
	foreach(SurfacePair surface, xff.getSurfaces()){
		TiXmlElement *submesh = new TiXmlElement("submesh");
		submeshes->LinkEndChild(submesh);

		submesh->SetAttribute("material", xff.basename()
			 + "/" + surface.second.getName());
		submesh->SetAttribute("operationtype", "triangle_list");
		submesh->SetAttribute("usesharedvertices", "false");

		TiXmlElement *faces = new TiXmlElement("faces");
		submesh->LinkEndChild(faces);

		surface.second.constructTriangleList();
		foreach(const Triangle &triangle, surface.second.getTriangles()){
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

		std::vector<Vertex> vertexList;
		vertexList.resize(surface.second.getUniqueMap().size());
		foreach(const UniqueMap::value_type &pair, surface.second.getUniqueMap()){
			vertexList.at(pair.second) = pair.first;
		}

		foreach(Vertex v, vertexList){
			TiXmlElement *vertex = new TiXmlElement("vertex");
			vertexbuffer->LinkEndChild(vertex);
			vertexbuffer->SetAttribute("positions", "true");	

			TiXmlElement *position = new TiXmlElement("position");
			vertex->LinkEndChild(position);

			position->SetDoubleAttribute("x", v.getPosition().x);
			position->SetDoubleAttribute("y", v.getPosition().y);
			position->SetDoubleAttribute("z", v.getPosition().z);

			if(v.hasNormal){
				TiXmlElement *normal = new TiXmlElement("normal");
				vertex->LinkEndChild(normal);
				normal->SetDoubleAttribute("x", v.getNormal().x);
				normal->SetDoubleAttribute("y", v.getNormal().y);
				normal->SetDoubleAttribute("z", v.getNormal().z);
				vertexbuffer->SetAttribute("normals", "true");
			}

			vertexbuffer->SetAttribute("colours_diffuse", "true");
			TiXmlElement *colour = new TiXmlElement("colour_diffuse");
			vertex->LinkEndChild(colour);
			Colour c = v.getColour();
			
			std::ostringstream o;
			o << c.r/256.0 << " " << c.g/256.0 << " " << c.b/256.0 << " " << c.a/256.0;
			colour->SetAttribute("value", o.str());

			if(v.hasTexture){
				TiXmlElement *texture = new TiXmlElement("texcoord");
				vertexbuffer->SetAttribute("texture_coords", 1);
				//vertexbuffer->SetAttribute("texture_coord_dimensions_0", 2);
				vertex->LinkEndChild(texture);
				texture->SetDoubleAttribute("u", v.getUvMap().u * 0.000244140625);
				texture->SetDoubleAttribute("v", v.getUvMap().v * 0.000244140625);
			}


		}

		TiXmlElement *submeshname = new TiXmlElement("submeshname");
		submeshnames->LinkEndChild(submeshname);
		submeshname->SetAttribute("name", surface.second.getName());
		submeshname->SetAttribute("index", surface.first);


	}
	doc.SaveFile(xff.filename + ".mesh.xml");
}



void Writer::generateMaterials(Xff &xff){
	std::ofstream material((xff.basename() + ".material").c_str());
	if(material.is_open()){
		foreach(SurfacePair pair, xff.getSurfaces()){
		material << "material " << xff.basename() << "/" << pair.second.getName() << '\n'
			<< "{" << '\n' 
			<< "\ttechnique" << '\n' 
			<< "\t{" << '\n'
			<< "\t\tpass" << '\n'
			<< "\t\t{" << '\n';
			if(pair.second.texture[0] - 1 > 0){
			std::cout << "\t\t\ttexture_unit" << '\n' 
			<< "\t\t\t{" << '\n' 
			<< "\t\t\t\t" << "texture\t" << xff.getTextures().at(pair.second.texture[0] - 1) << ".png" << '\n'
			<< "\t\t\t}" << '\n';
			}
			std::cout << "\t\t}" << '\n' 
			<< "\t}" << '\n' 
			<< "}" << '\n' << '\n';
		}
	}
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
		Writer out(xff);
	}
}else{
	Xff xff( "/home/foobat/dormin/reeng/knight_A.nmo");
	Writer out(xff);
}
return 0;
}
