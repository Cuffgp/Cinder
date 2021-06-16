#include "cnpch.h"
#include "VulkanVertexBuffer.h"
#include "Cinder/Core/Application.h"

namespace Cinder {

	VertexBuffer::VertexBuffer(void* data, uint32_t size) :
		m_Size(size)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_Size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		//bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		m_LocalData = Buffer::Copy(data, size);
		m_Allocation = VulkanAllocator::AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, m_VulkanBuffer);

		void* dst = VulkanAllocator::MapMemory(m_Allocation);
		memcpy(dst, m_LocalData.Data, m_Size);
		VulkanAllocator::UnmapMemory(m_Allocation);
	}

	VertexBuffer::~VertexBuffer()
	{
		VulkanAllocator::DestroyBuffer(m_VulkanBuffer, m_Allocation);
	}

}