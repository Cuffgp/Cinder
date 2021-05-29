#pragma once

#include "Cinder/Core/Window.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include "VulkanUniformBuffer.h"

namespace Cinder {

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class VulkanRenderer
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();

		VkRenderPass getSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			CN_CORE_ASSERT(isFrameStarted, "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		UniformBuffer& getCurrentUniformBuffer() { return uniformBuffers[currentFrameIndex]; }

		int getFrameIndex() const {
			CN_CORE_ASSERT(isFrameStarted, "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		void OnWindowResize(uint32_t width, uint32_t height);

		Ref<VulkanSwapChain> getSwapChain() { return m_SwapChain; }
		Ref<VulkanDevice> getDevice() { return m_Device; }
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		Ref<VulkanDevice> m_Device;
		Ref<VulkanSwapChain> m_SwapChain;

		std::vector<VkCommandBuffer> commandBuffers;

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<UniformBuffer> uniformBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};

}