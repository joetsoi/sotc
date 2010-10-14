#include "Model.h"

using namespace sotc;

Model& Model::addTexture(const Texture &texture){
	textures.push_back(texture);
	return *this;
}



Model& Model::addSurface(uint32_t index, const Surface &surface){
	surfaces.insert(std::pair<uint32_t, Surface>(index, surface));
	return *this;
}
