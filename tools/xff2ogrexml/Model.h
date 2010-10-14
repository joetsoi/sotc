#ifndef Model_h
#define Model_h

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include "Surface.h"
#include "Texture.h"

namespace sotc{
	class Model{
	public:
		Model(const std::string &name) : name(name) {}
		Model& addSurface(uint32_t index, const Surface &surface);
		Model& addTexture(const Texture &texture);
		std::map<uint32_t, Surface>& getSurfaces() { return surfaces; }
		Surface& getSurface(uint32_t index) { return surfaces[index]; }
		std::vector<Texture>& getTextures() { return textures; }
		Texture& getTexture(uint32_t index) { return textures.at(index); }
		std::string name;
	private:
		std::vector<Texture> textures;
		std::map<uint32_t, Surface> surfaces;
	};
}

#endif
