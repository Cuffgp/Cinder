#include "cnpch.h"
#include "Cinder/Core/Application.h"

#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Cinder {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		CN_CORE_ASSERT(!s_Instance, "Application already exsists!")
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetEventCallback(CN_BIND_EVENT_FN(OnEvent));

		m_Device = CreateRef<VulkanDevice>(m_Window->GetNativeWindow());
		m_Renderer = CreateRef<VulkanRenderer>(m_Window, m_Device);

		loadModels();
		createPipelineLayout();
		createPipeline(m_Renderer->getSwapChainRenderPass());

		m_ImGuiLayer = new ImGuiLayer(m_Renderer, m_Window);
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(CN_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(CN_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);

				m_Window->OnUpdate();
				if (auto commandBuffer = m_Renderer->beginFrame())
				{
					m_Renderer->beginSwapChainRenderPass(commandBuffer);
					renderGameObjects(commandBuffer);
					m_ImGuiLayer->Begin();
					m_ImGuiLayer->End(commandBuffer);
					m_Renderer->endSwapChainRenderPass(commandBuffer);

					m_Renderer->endFrame();
				}

			}
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Renderer->OnWindowResize(e.GetWidth(), e.GetHeight());

		m_Minimized = false;

		return false;
	}

	void Application::loadModels()
	{
		std::vector<VulkanModel::Vertex> vertices
		{
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
		};

		std::vector<uint32_t> indices
		{ 0, 1, 2, 2, 3, 0 };

		m_Model = CreateScope<VulkanModel>(m_Device, vertices, indices);
		//m_Model = CreateScope<VulkanModel>(m_Device, "assets/objects/Spot.obj");
	}

	void Application::createPipelineLayout()
	{
		//VkPushConstantRange pushConstantRange{};
		//pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		//pushConstantRange.offset = 0;
		//pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		//pipelineLayoutInfo.pushConstantRangeCount = 1;
		//pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(m_Device->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void Application::createPipeline(VkRenderPass renderPass)
	{
		CN_ASSERT(pipelineLayout, "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		m_Pipeline = std::make_unique<VulkanPipeline>(
			m_Device,
			"assets/shaders/shader.vert.spv",
			"assets/shaders/shader.frag.spv",
			pipelineConfig);
	}

	/*
	void Application::recordCommandBuffer(VkCommandBuffer commandBuffer)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->getRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_SwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		m_Pipeline->bind(commandBuffers[imageIndex]);
		m_Model->bind(commandBuffers[imageIndex]);
		m_Model->draw(commandBuffers[imageIndex]);

		
		SimplePushConstantData push{};
		//push.projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		//push.view = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));

		vkCmdPushConstants(
			commandBuffers[imageIndex],
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&push);
		

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
			CN_CORE_ERROR("failed to record command buffer!");
	}
	*/

	void Application::renderGameObjects(VkCommandBuffer commandBuffer)
	{
		m_Pipeline->bind(commandBuffer);
		m_Model->bind(commandBuffer);
		m_Model->draw(commandBuffer);
	}

	
}
