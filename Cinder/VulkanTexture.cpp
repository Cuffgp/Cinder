#include "cnpch.h"

#include "VulkanTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Cinder {

	Texture::Texture(const std::string& texFilepath)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(texFilepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		uint32_t bufferSize = texWidth * texHeight * 4;

		if (!pixels)
			CN_ERROR("failed to load texture image!");
	}

}