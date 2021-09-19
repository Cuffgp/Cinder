#pragma once

#include "Vulkan.h"
#include "VulkanAllocator.h"

namespace Cinder {

	class Texture
	{
	public:
		Texture(void* data, uint32_t width, uint32_t height);
		Texture(const std::string& texFilepath);
		~Texture();

		VkImageView getView() { return textureImageView; }
		VkSampler getSampler() { return textureSampler; }

	private:

		void createTexture(const std::string& texFilepath);
		void createTexture(void* data, uint32_t width, uint32_t height);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void createTextureImageView();
		void createTextureSampler();

		VkImage m_Image;
		VmaAllocation m_ImageAllocation;

		VkImageView textureImageView;
		VkSampler textureSampler;

	};

}