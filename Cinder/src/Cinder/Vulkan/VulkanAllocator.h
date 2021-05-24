#pragma once

#include "Vulkan.h"
#include "vk_mem_alloc.h"

namespace Cinder {

	class VulkanAllocator
	{
	public:
		static void Init();
		static void Shutdown();

		VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VkBuffer& outBuffer);
		VmaAllocation AllocateImage(VkImageCreateInfo imageCreateInfo, VkImage& outImage);

	private:
		static VmaAllocator s_Allocator;
	};

}