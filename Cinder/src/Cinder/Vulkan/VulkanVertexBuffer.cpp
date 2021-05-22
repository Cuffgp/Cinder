#include "cnpch.h"
#include "VulkanVertexBuffer.h"
#include "Cinder/Core/Application.h"

namespace Cinder {

	VertexBuffer::VertexBuffer(void* data, uint32_t size) :
		m_Size(size)
	{
		auto device = Application::Get().GetVulkanDevice();

		m_LocalData = Buffer::Copy(data, size);

		device->createBuffer(
			size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory);

	}

	VertexBuffer::~VertexBuffer()
	{

	}

}