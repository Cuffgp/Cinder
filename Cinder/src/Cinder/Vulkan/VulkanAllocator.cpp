#include "cnpch.h"
#include "VulkanAllocator.h"
#include "Cinder/Core/Application.h"

namespace Cinder {

	VmaAllocator VulkanAllocator::s_Allocator = nullptr;

	void VulkanAllocator::Init()
	{
		auto device = Application::Get().GetVulkanDevice();

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
		allocatorInfo.physicalDevice = device->getPhysicalDevice();
		allocatorInfo.device = device->device();
		allocatorInfo.instance = device->getInstance();

		vmaCreateAllocator(&allocatorInfo, &s_Allocator);
	}

	void VulkanAllocator::Shutdown()
	{
		vmaDestroyAllocator(s_Allocator);
	}

	VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VkBuffer& outBuffer)
	{
		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VmaAllocation allocation;
		VulkanCheckResult(vmaCreateBuffer(s_Allocator, &bufferCreateInfo, &allocCreateInfo, &outBuffer, &allocation, nullptr));
		return allocation;
	}

	VmaAllocation VulkanAllocator::AllocateImage(VkImageCreateInfo imageCreateInfo, VkImage& outImage)
	{
		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VmaAllocation allocation;
		vmaCreateImage(s_Allocator, &imageCreateInfo, &allocCreateInfo, &outImage, &allocation, nullptr);

		return allocation;
	}

	void VulkanAllocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
	{
		CN_CORE_ASSERT(buffer);
		CN_CORE_ASSERT(allocation);
		vmaDestroyBuffer(s_Allocator, buffer, allocation);
	}

	void VulkanAllocator::DestroyImage(VkImage image, VmaAllocation allocation)
	{
		CN_CORE_ASSERT(image);
		CN_CORE_ASSERT(allocation);
		vmaDestroyImage(s_Allocator, image, allocation);
	}

	void* VulkanAllocator::MapMemory(VmaAllocation allocation)
	{
		void* mappedMemory;
		vmaMapMemory(s_Allocator, allocation, (void**)&mappedMemory);
		return mappedMemory;
	}

	void VulkanAllocator::UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(s_Allocator, allocation);
	}
}