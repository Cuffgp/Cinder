#pragma once

#include "Cinder/Core/Buffer.h"
#include "Cinder/Renderer/BufferLayout.h"
#include "VulkanAllocator.h"

#include "Vulkan.h"


namespace Cinder {

	class VertexBuffer
	{
	public:
		VertexBuffer(void* data, uint32_t size);
		~VertexBuffer();

		VkBuffer GetVulkanBuffer() { return m_VulkanBuffer; }
	private:
		uint32_t m_Size = 0;
		Buffer m_LocalData;

		VkBuffer m_VulkanBuffer = nullptr;
		VmaAllocation m_Allocation;
	};

}