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

		VkBuffer GetVulkanBuffer() { return m_VulkanBuffer; }
		uint32_t GetIndexCount() { return m_IndexCount; }
	private:
		uint32_t m_Size = 0;
		uint32_t m_IndexCount = 0;
		Buffer m_LocalData;

		VkBuffer m_VulkanBuffer = nullptr;
		VmaAllocation m_Allocation;
	};

}