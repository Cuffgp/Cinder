#pragma once

#include "Cinder/Core/Buffer.h"

#include "Vulkan.h"
#include "VulkanAllocator.h"


namespace Cinder {

	class UniformBuffer
	{
	public:
		UniformBuffer(uint32_t size);
		~UniformBuffer();

		VkBuffer GetVulkanBuffer() { return m_VulkanBuffer; }
		void SetData(void* data, uint32_t size);
	private:
		uint32_t m_Size = 0;
		Buffer m_LocalData;

		VkBuffer m_VulkanBuffer = nullptr;
		VmaAllocation m_Allocation;
	};

}