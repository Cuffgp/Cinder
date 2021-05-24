#pragma once

#include "Vulkan.h"
#include "vk_mem_alloc.h"

namespace Cinder {

	class VulkanAllocator
	{
	public:
		static void Init();
		static void Shutdown();

		static VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VkBuffer& outBuffer);
		static VmaAllocation AllocateImage(VkImageCreateInfo imageCreateInfo, VkImage& outImage);
		static void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);
		static void DestroyImage(VkImage image, VmaAllocation allocation);
		static void MapMemory(VmaAllocation allocation, void* data);
		static void UnmapMemory(VmaAllocation allocation);


	private:
		static VmaAllocator s_Allocator;
	};

}