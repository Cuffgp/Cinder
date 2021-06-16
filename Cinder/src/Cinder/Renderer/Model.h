#pragma once

#include <glm/glm.hpp>
#include "Cinder/Core/Base.h"
#include "Cinder/Vulkan/VulkanVertexBuffer.h"
#include "Cinder/Vulkan/VulkanIndexBuffer.h"

namespace Cinder {

	class Model {
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec2 texCoord;
		};

		Model(const std::string& objFilepath);
		~Model();

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

	};

}