#include "Skeleton.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include <OgreMath.h>
#include <OgreStringConverter.h>
#include "tinyxml.h"

typedef Ogre::StringConverter Stringify;

namespace sotc{
	class Writer{
	public:
		Writer(const Skeleton &skeleton, const std::string &destination){
			TiXmlDocument doc;
			TiXmlElement *skeletonElement = new TiXmlElement("skeleton");
			doc.LinkEndChild(skeletonElement);
			TiXmlElement *bonesElement = new TiXmlElement("bones");
			skeletonElement->LinkEndChild(bonesElement);
			TiXmlElement *boneheirarchy = new TiXmlElement("boneheirarchy");
			skeletonElement->LinkEndChild(boneheirarchy);


			TiXmlElement *root = new TiXmlElement("bone");
			root->SetAttribute("id", "0");
			root->SetAttribute("name", "root");
			bonesElement->LinkEndChild(root);
			int count = 0; //ugh laziness
			foreach(const Bone &bone, skeleton.getBones()){
				TiXmlElement *boneElement = new TiXmlElement("bone");
				boneElement->SetAttribute("id", count);
				boneElement->SetAttribute("name", bone.getName());
				bonesElement->LinkEndChild(boneElement);
				TiXmlElement *position = new TiXmlElement("position");
				position->SetAttribute("x", Stringify::toString(bone.getPosition().x));
				position->SetAttribute("y", Stringify::toString(bone.getPosition().y));
				position->SetAttribute("z", Stringify::toString(bone.getPosition().z));
				boneElement->LinkEndChild(position);

				TiXmlElement *rotation = new TiXmlElement("rotation");

				Ogre::Radian angle;
				Ogre::Vector3 axis;
				bone.getRotation().ToAngleAxis(angle, axis);
				rotation->SetAttribute("angle", Stringify::toString(angle));
				boneElement->LinkEndChild(rotation);

				TiXmlElement *axisElement = new TiXmlElement("axis");
				axisElement->SetAttribute("x", Stringify::toString(axis.x));
				axisElement->SetAttribute("y", Stringify::toString(axis.y));
				axisElement->SetAttribute("z", Stringify::toString(axis.z));
				rotation->LinkEndChild(axisElement);
				if(bone.getParent() == -1){
					TiXmlElement *boneparent = new TiXmlElement("boneparent");
					boneparent->SetAttribute("name", bone.getName());
					boneparent->SetAttribute("parent", "root");
					boneheirarchy->LinkEndChild(boneparent);
				} else {
					TiXmlElement *boneparent = new TiXmlElement("boneparent");
					boneparent->SetAttribute("name", bone.getName());
					boneparent->SetAttribute("parent", skeleton.getBones().at(count - 1).getName());
					boneheirarchy->LinkEndChild(boneparent);
				}
				count++;
			}
			doc.SaveFile(destination);
		}
	};
}



int main(int argc, char* argv[]){
	if(argc == 3){
		std::cout << "test" << '\n';
		sotc::Skeleton skeleton(argv[1]);
		sotc::Writer writer(skeleton, argv[2]);
		return 0;
	}
//boost::iostreams::mapped_file test("/home/foobat/dormin/reeng/knight_A.skb");
//std::cout << test.size() << '\n';
//const int* offset = reinterpret_cast<const int*>(test.const_data() + 0x4c);
//const int* rodataSize = reinterpret_cast<const int*>(test.const_data() + 0xa0);
//const int* rodata = reinterpret_cast<const int*>(test.const_data() + 0xb4);
//std::cout << *rodata << '\n';
}
