#include "cnpch.h"
#include "VulkanRenderer.h"

namespace Cinder {

	VulkanRenderer::VulkanRenderer(Ref<Window> window, Ref<VulkanDevice> device)
		:m_Window(window), m_Device(device)
	{
		recreateSwapChain();
		createCommandBuffers();
	}

    VulkanRenderer::~VulkanRenderer()
    {
        freeCommandBuffers();
    }

	void VulkanRenderer::recreateSwapChain()
	{
		auto width = m_Window->GetWidth();
		auto height = m_Window->GetHeight();

		VkExtent2D extent = { width, height };

		vkDeviceWaitIdle(m_Device->device());

		if (m_SwapChain == nullptr)
		{
			m_SwapChain = CreateScope<VulkanSwapChain>(m_Device, extent);
		}
		else
		{
            Ref<VulkanSwapChain> oldSwapChain = std::move(m_SwapChain);
			m_SwapChain = CreateRef<VulkanSwapChain>(m_Device, extent, oldSwapChain);
			if (!oldSwapChain->compareSwapFormats(*m_SwapChain.get()))
			{
                CN_ERROR("Swap chain image(or depth) format has changed!");
			}
		}
		//createPipeline();
	}

	void VulkanRenderer::createCommandBuffers()
	{
		commandBuffers.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_Device->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device->device(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void VulkanRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device->device(), m_Device->getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}


    VkCommandBuffer VulkanRenderer::beginFrame() {
        CN_CORE_ASSERT(!isFrameStarted, "Can't call beginFrame while already in progress");

        auto result = m_SwapChain->acquireNextImage(&currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            CN_CORE_ERROR("VK_ERROR_OUT_OF_DATE_KHR");
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            CN_CORE_ERROR("failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            CN_CORE_ERROR("failed to begin recording command buffer!");
        }
        return commandBuffer;
    }

    void VulkanRenderer::endFrame() {
        CN_CORE_ASSERT(isFrameStarted, "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            CN_CORE_ERROR("failed to record command buffer!");
        }

        auto result = m_SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            CN_CORE_ERROR("failed to present swap chain image!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        CN_CORE_ASSERT(isFrameStarted, "Can't call beginSwapChainRenderPass if frame is not in progress");
        CN_CORE_ASSERT(
            commandBuffer == getCurrentCommandBuffer(),
            "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, m_SwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkanRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        CN_CORE_ASSERT(isFrameStarted, "Can't call endSwapChainRenderPass if frame is not in progress");
        CN_CORE_ASSERT(
            commandBuffer == getCurrentCommandBuffer(),
            "Can't end render pass on command buffer from a different frame");
        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanRenderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        //recreateSwapChain();
    }
}