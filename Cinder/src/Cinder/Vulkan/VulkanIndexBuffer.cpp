#include "cnpch.h"
#include "VulkanIndexBuffer.h"
#include "Cinder/Core/Application.h"

namespace Cinder {

	IndexBuffer::IndexBuffer(void* data, uint32_t size)
	{
		auto device = Application::Get().GetVulkanDevice();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_Size;
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		m_LocalData = Buffer::Copy(data, size);
		m_Allocation = VulkanAllocator::AllocateBuffer(bufferInfo, m_VulkanBuffer);

		void* dst;
		VulkanAllocator::MapMemory(m_Allocation, dst);
		memcpy(dst, m_LocalData.Data, m_Size);
		VulkanAllocator::UnmapMemory(m_Allocation);
	}

	IndexBuffer::~IndexBuffer()
	{
		VulkanAllocator::DestroyBuffer(m_VulkanBuffer, m_Allocation);
	}

}