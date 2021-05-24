#include "cnpch.h"
#include "VulkanAllocator.h"
#include "Cinder/Core/Application.h"

namespace Cinder {

	VmaAllocator VulkanAllocator::s_Allocator = nullptr;

	void VulkanAllocator::Init()
	{
		auto device = Application::Get().GetVulkanDevice();

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
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
		vmaCreateBuffer(s_Allocator, &bufferCreateInfo, &allocCreateInfo, &outBuffer, &allocation, nullptr);
	}

	VmaAllocation VulkanAllocator::AllocateImage(VkImageCreateInfo imageCreateInfo, VkImage& outImage)
	{

	}
}