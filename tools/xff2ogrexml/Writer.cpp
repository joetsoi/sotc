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
Writer::Writer(Model &model, bool hasDestination, std::string outputDirectory)
		: hasDestination(hasDestination), outputDirectory(outputDirectory)
{
	generateXml(model);
	generateMaterials(model);
}

void Writer::generateXml(Model &model){
	TiXmlDocument doc;
	TiXmlElement *mesh = new TiXmlElement("mesh");
	doc.LinkEndChild(mesh);

	TiXmlElement *submeshes = new TiXmlElement("submeshes");
	mesh->LinkEndChild(submeshes);

	TiXmlElement *submeshnames = new TiXmlElement("submeshnames");
	mesh->LinkEndChild(submeshnames);
	
	foreach(SurfacePair surface, model.getSurfaces()){
		if(surface.second.getTriangleCount() != 0){
			TiXmlElement *submesh = new TiXmlElement("submesh");
			submeshes->LinkEndChild(submesh);

			submesh->SetAttribute("material", model.name + "/" + surface.second.getName());
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
					texture->SetDoubleAttribute("u", v.getUvMap().x);
					texture->SetDoubleAttribute("v", v.getUvMap().y);
				} else if(v.hasTexture){
					TiXmlElement *texture = new TiXmlElement("texcoord");
					vertexbuffer->SetAttribute("texture_coords", 1);
					//vertexbuffer->SetAttribute("texture_coord_dimensions_0", 2);
					vertex->LinkEndChild(texture);
					texture->SetDoubleAttribute("u", v.getUvMap().x);
					texture->SetDoubleAttribute("v", v.getUvMap().y);
				}
			}

			TiXmlElement *submeshname = new TiXmlElement("submeshname");
			submeshnames->LinkEndChild(submeshname);
			submeshname->SetAttribute("name", surface.second.getName());
			submeshname->SetAttribute("index", surface.first);


		}
	}
	std::string filename;
	if(hasDestination){
		filename = outputDirectory + "/" + model.name + ".mesh.xml";
	} else {
		filename = model.name + ".mesh.xml";
	}
	doc.SaveFile(filename);
}



void Writer::generateMaterials(Model &model){
	std::string filename;
	if(hasDestination)
		filename = outputDirectory + "/" + model.name + ".material";
	else
		filename = model.name + ".material";
	std::ofstream material(filename.c_str());
	std::cout << filename << '\n';
	if(material.is_open()){
		foreach(SurfacePair pair, model.getSurfaces()){
		material << "material " << model.name << "/" << pair.second.getName() << '\n'
			<< "{" << '\n' 
			<< "\ttechnique" << '\n' 
			<< "\t{" << '\n'
			<< "\t\tpass" << '\n'
			<< "\t\t{" << '\n';
		if(model.getTexture(pair.second.texture[1]).isTransparent == 1){
			material << "\t\t\tscene_blend alpha_blend" << '\n';
		}
		material << "\t\t\ttexture_unit" << '\n' 
			<< "\t\t\t{" << '\n' 
			//<< "\t\t\t\talpha_op_ex add src_diffuse src_texture" << '\n' 
			<< "\t\t\t\t" << "texture\t" << model.getTexture(pair.second.texture[1]).name << ".png" << '\n'
			<< "\t\t\t}" << '\n'
			<< "\t\t}" << '\n' 
			<< "\t}" << '\n' 
			<< "}" << '\n' << '\n';
		}
		material.close();
	}
}



int main(int argc, char *argv[]){
//	std::ifstream xff(argv[1], std::ios::binary);
//	std::vector<char> buffer;
//	xff >> std::noskipws;
//	typedef std::istream_iterator<char> istream_iterator;
//	std::copy(istream_iterator(xff), istream_iterator(), std::back_inserter(buffer));
	if(argc == 2){
		Xff xff(argv[1]);
		Writer out(xff.getModel());
	} else if(argc == 3){
		Xff xff(argv[1]);
		Writer out(xff.getModel(), true, std::string(argv[2]));
	} else {
		std::cout << "usage xff2ogrexml source [dest]" << std::endl;
	}
	return 0;
}
