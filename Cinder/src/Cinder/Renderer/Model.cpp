#include "cnpch.h"
#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Cinder {

	Model::Model(const std::string& objFilepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFilepath.c_str()))
		{
			CN_CORE_ERROR(err);
		}
		/*
		CN_CORE_INFO("# of vertices  = {}", (int)(attrib.vertices.size()) / 3);
		CN_CORE_INFO("# of normals   = {}", (int)(attrib.normals.size()) / 3);
		CN_CORE_INFO("# of texcoords = {}", (int)(attrib.texcoords.size()) / 2);
		CN_CORE_INFO("# of materials = {}", (int)materials.size());
		CN_CORE_INFO("# of shapes    = {}", (int)shapes.size());
		*/
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

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

				vertex.color = { 0.6f, 0.4f , 0.8f };

				vertices.push_back(vertex);
				indices.push_back(indices.size());
			}
		}

		m_VertexBuffer = CreateRef<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
		m_IndexBuffer = CreateRef<IndexBuffer>(indices.data(), indices.size() * sizeof(uint32_t));
	}

	Model::~Model()
	{

	}

	void Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_VertexBuffer->GetVulkanBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetVulkanBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void Model::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, m_IndexBuffer->GetIndexCount(), 1, 0, 0, 0);
	}


}