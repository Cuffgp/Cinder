#include "cnpch.h"
#include "VulkanModel.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// std
#include <cassert>
#include <cstring>

namespace Cinder {

	VulkanModel::VulkanModel(Ref<VulkanDevice> device, const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indices) : m_Device{ device }
	{
		createVertexBuffers(vertices);
		createIndexBuffers(indices);
	}

	VulkanModel::VulkanModel(Ref<VulkanDevice> device, const std::string& objFilepath) : m_Device{ device }
	{

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFilepath.c_str())) 
		{
			CN_CORE_ERROR(err);
		}

		for (const auto& shape : shapes) 
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				vertex.position = 
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.color = { 0.8f, 0.4f , 0.6f };

				vertices.push_back(vertex);
				indices.push_back(indices.size());
			}
		}

		createVertexBuffers(vertices);
		createIndexBuffers(indices);
	}

	VulkanModel::~VulkanModel()
	{
		vkDestroyBuffer(m_Device->device(), vertexBuffer, nullptr);
		vkFreeMemory(m_Device->device(), vertexBufferMemory, nullptr);

		vkDestroyBuffer(m_Device->device(), indexBuffer, nullptr);
		vkFreeMemory(m_Device->device(), indexBufferMemory, nullptr);
	}

	void VulkanModel::createVertexBuffers(const std::vector<Vertex>& vertices) 
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		m_Device->createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory);

		void* data;
		vkMapMemory(m_Device->device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_Device->device(), vertexBufferMemory);
	}

	void VulkanModel::createIndexBuffers(const std::vector<uint32_t>& indicies)
	{
		indexCount = static_cast<uint32_t>(indicies.size());
		VkDeviceSize bufferSize = sizeof(indicies[0]) * indexCount;
		m_Device->createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			indexBuffer,
			indexBufferMemory);

		void* data;
		vkMapMemory(m_Device->device(), indexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indicies.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_Device->device(), indexBufferMemory);
	}

	void VulkanModel::draw(VkCommandBuffer commandBuffer) 
	{
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}

	void VulkanModel::bind(VkCommandBuffer commandBuffer) 
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	std::vector<VkVertexInputBindingDescription> VulkanModel::Vertex::getBindingDescriptions() 
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> VulkanModel::Vertex::getAttributeDescriptions() 
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

}