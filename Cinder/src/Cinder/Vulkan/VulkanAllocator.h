#pragma once

#include "Cinder/Core/Base.h"

#include "Vulkan.h"
#include "vk_mem_alloc.h"

namespace Cinder {

	class VulkanAllocator
	{
	public:
		static void Init();
		static void Shutdown();

		static VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VmaMemoryUsage usage, VkBuffer& outBuffer);
		static VmaAllocation AllocateImage(VkImageCreateInfo imageCreateInfo, VmaMemoryUsage usage, VkImage& outImage);
		static void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);
		static void DestroyImage(VkImage image, VmaAllocation allocation);
		static void* MapMemory(VmaAllocation allocation);
		static void UnmapMemory(VmaAllocation allocation);


	private:
		static VmaAllocator s_Allocator;
	};

}