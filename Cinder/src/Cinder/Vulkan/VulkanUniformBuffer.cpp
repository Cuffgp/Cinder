#include "cnpch.h"
#include "VulkanUniformBuffer.h"

namespace Cinder {

	UniformBuffer::UniformBuffer(uint32_t size):
		m_Size(size)
	{
		CN_CORE_INFO("Creating Uniform Buffer, size {}", size);
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_Size;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		//bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		m_Allocation = VulkanAllocator::AllocateBuffer(bufferInfo, m_VulkanBuffer);
	}

	UniformBuffer::~UniformBuffer()
	{
		VulkanAllocator::DestroyBuffer(m_VulkanBuffer, m_Allocation);
	}

	void UniformBuffer::SetData(void* data, uint32_t size)
	{
		CN_CORE_ASSERT(size == m_Size, "Uniform data must have same size as buffer.");
		void* dst = VulkanAllocator::MapMemory(m_Allocation);
		memcpy(dst, data, size);
		VulkanAllocator::UnmapMemory(m_Allocation);
	}

}