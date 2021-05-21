#pragma once

#include "Vulkan.h"
#include "VulkanDevice.h"

#include "Cinder/Core/Base.h"

#include <glm/glm.hpp>


// std
#include <vector>

namespace Cinder {

	class VulkanModel 
	{
	public:
		struct Vertex 
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		VulkanModel(Ref<VulkanDevice> device, const std::vector<Vertex>& vertices,
			const std::vector<uint32_t>& indices);
		VulkanModel(Ref<VulkanDevice> device, const std::string& objFilepath);
		~VulkanModel();

		VulkanModel(const VulkanModel&) = delete;
		VulkanModel& operator=(const VulkanModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		Ref<VulkanDevice> m_Device;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;

	};
}
