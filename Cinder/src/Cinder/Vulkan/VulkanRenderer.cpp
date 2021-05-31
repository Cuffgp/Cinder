#include "cnpch.h"
#include "VulkanRenderer.h"
#include "VulkanAllocator.h"
#include "Cinder/Core/Application.h"

namespace Cinder {

	VulkanRenderer::VulkanRenderer()
	{
		//VulkanAllocator::Init();

		m_Device = Application::Get().GetVulkanDevice();
		recreateSwapChain();
		createCommandBuffers();
		createDescriptorObjects();

	}

	VulkanRenderer::~VulkanRenderer()
	{
		VulkanAllocator::Shutdown();

		freeCommandBuffers();
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

	void VulkanRenderer::createDescriptorObjects()
	{
		// Uniform Buffers
		uniformBuffers.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
			uniformBuffers[i] = CreateRef<UniformBuffer>(sizeof(UniformBufferObject));

		// Descriptor Pool
		std::array<VkDescriptorPoolSize, 1> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		//poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//poolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChain->imageCount());

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(m_Device->device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			CN_CORE_ERROR("failed to create descriptor pool!");

		// Descriptor set layout (should come from shader later?)
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_Device->device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			CN_CORE_ERROR("failed to create descriptor set layout!");

		// Descriptor sets
		std::vector<VkDescriptorSetLayout> layouts(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(m_SwapChain->imageCount());
		if (vkAllocateDescriptorSets(m_Device->device(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
			CN_CORE_ERROR("failed to allocate descriptor sets!");

		for (size_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i]->GetVulkanBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			/*
			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;
			*/

			vkUpdateDescriptorSets(m_Device->device(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

	}

	void VulkanRenderer::recreateSwapChain()
	{
		auto width = Application::Get().GetWindow().GetWidth();
		auto height = Application::Get().GetWindow().GetHeight();

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
	}

	VkCommandBuffer VulkanRenderer::beginFrame() {
		CN_CORE_ASSERT(!isFrameStarted, "Can't call beginFrame while already in progress");

		auto result = m_SwapChain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) 
		{
			CN_CORE_ERROR("VK_ERROR_OUT_OF_DATE_KHR");
			recreateSwapChain();
			beginFrame();
			//return nullptr;
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