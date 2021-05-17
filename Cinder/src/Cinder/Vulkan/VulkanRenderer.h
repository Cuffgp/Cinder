#pragma once

#include "Cinder/Core/Window.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

namespace Cinder {

	class VulkanRenderer
	{
	public:
		VulkanRenderer(Ref<Window>, Ref<VulkanDevice>);
		~VulkanRenderer();

		VkRenderPass getSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			CN_CORE_ASSERT(isFrameStarted, "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			CN_CORE_ASSERT(isFrameStarted, "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		void OnWindowResize(uint32_t width, uint32_t height);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		Ref<Window> m_Window;
		Ref<VulkanDevice> m_Device;
		Ref<VulkanSwapChain> m_SwapChain;

		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};

}