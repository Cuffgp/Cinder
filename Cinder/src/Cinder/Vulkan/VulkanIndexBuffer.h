#pragma once

#include "Vulkan.h"
#include "VulkanAllocator.h"
#include "Cinder/Core/Buffer.h"

namespace Cinder {

	class IndexBuffer
	{
	public:
		IndexBuffer(void* data, uint32_t size);
		~IndexBuffer();
	private:
		uint32_t m_Size = 0;
		Buffer m_LocalData;

		VkBuffer m_VulkanBuffer = nullptr;
		VmaAllocation m_Allocation;
	};

}